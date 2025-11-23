using System;
using System.Collections.Generic;
using System.Threading;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Manages a client connection to the messaging mesh.
    /// </summary><remarks>
    /// Note about multiple subsciptions to the same subject
    /// ----------------------------------------------------
    /// If multiple subscriptions are made to the same subject we only make one subscription
    /// to the gateway for that subject. The multiple subscriptions are handled here at the 
    /// client end. If we receive an update we route it to each callback (including any 
    /// callback-speific tag). We unsubscribe from the gateway when all subscriptions have
    /// been removed.
    /// </remarks>
    public class Connection : ClientSocket.ICallback
    {
        #region Public types

        /// <summary>
        /// Definition of the subscription callback.
        /// </summary>
        public delegate void SubscriptionCallback(Connection connection, string subject, string replySubject, Message message, object tag);

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Connection(ConnectionParams connectionParams)
        {
            m_connectionParams = connectionParams;

            // We create the socket connection to the gateway and listen for updates from it...
            m_clientSocket = new ClientSocket(connectionParams.GatewayHost, connectionParams.GatewayPort, connectionParams.Service);
            m_clientSocket.setCallback(this);

            // We send a CONNECT message...
            var networkMessage = new NetworkMessage();
            networkMessage.Header.Action = NetworkMessageHeader.ActionEnum.CONNECT;
            networkMessage.Header.Subject = connectionParams.Service;
            sendNetworkMessage(networkMessage);

            // We wait for the ACK to confirm that we have connected.
            //
            // This is done in the constructor as we do not want client code to send messages
            // until the Gateway has fully set up the socket at its end and assigned it to
            // the correct thread for the requested service. When the ACK signal has been sent
            // we know that this has been completed.
            var waitResult = m_ackSignal.WaitOne(millisecondsTimeout: 30000);
            if (!waitResult)
            {
                throw new MessagingMeshException("Timed out without receive ACK from the Messaging Mesh Gateway");
            }
        }

        /// <summary>
        /// Sends a message to the specified subject.
        /// </summary>
        public void sendMessage(string subject, Message message)
        {
            // We create a NetworkMessage to send the message...
            var networkMessage = new NetworkMessage();
            networkMessage.Header.Action = NetworkMessageHeader.ActionEnum.SEND_MESSAGE;
            networkMessage.Header.Subject = subject;
            networkMessage.Message = message;

            // We send the message...
            sendNetworkMessage(networkMessage);
        }

        /// <summary>
        /// Subscribes to the subject requested.
        /// 
        /// The callback will be called when updates are received for messages on the requested subject.
        /// Callbacks will include the tag.
        /// 
        /// The lifetime of the subscription is the lifetime of the returned Subscription object.
        /// </summary>
        public Subscription subscribe(string subject, SubscriptionCallback callback, object tag = null)
        {
            // We create the subscription-callback-info for this subscription...
            var callbackInfo = new Subscription.CallbackInfo
            {
                Callback = callback,
                Tag = tag
            };

            bool makeGatewaySubscription = false;
            SubscriptionInfo subscriptionInfo;
            lock (m_subscriptionsLocker)
            {
                // We check if we are already subscribed to this subject...
                if (!m_subscriptionsBySubject.TryGetValue(subject, out subscriptionInfo))
                {
                    // We are not subscribed to the subject, so we set up the subscription and
                    // note that we need to subscribe via the gateway...
                    subscriptionInfo = new SubscriptionInfo 
                    { 
                        Subject = subject,
                        SubscriptionID = (uint)Interlocked.Increment(ref m_subscriptionID)
                    };
                    m_subscriptionsBySubject.Add(subject, subscriptionInfo);
                    m_subscriptionsByID.Add(subscriptionInfo.SubscriptionID, subscriptionInfo);
                    makeGatewaySubscription = true;
                }

                // We add our subscription...
                subscriptionInfo.CallbackInfos.Add(callbackInfo);
            }

            // We subscribe to the gateway if needed... 
            if (makeGatewaySubscription)
            {
                // We send a SUBSCRIBE message...
                var networkMessage = new NetworkMessage();
                var header = networkMessage.Header;
                header.Action = NetworkMessageHeader.ActionEnum.SUBSCRIBE;
                header.SubscriptionID = subscriptionInfo.SubscriptionID;
                header.Subject = subject;
                sendNetworkMessage(networkMessage);
            }

            // We return a subscription object to manage the lifetime of the subscription...
            return new Subscription(this, subject, callbackInfo, subscriptionInfo.SubscriptionID);
        }

        /// <summary>
        /// Processes messages in the queue. Waits for the specified time for messages to be available.
        /// </summary>
        public void processMessageQueue(int millisecondsTimeout)
        {
            var queuedMessages = m_queuedMessages.waitAndGetItems(millisecondsTimeout);
            foreach(var queuedMessage in queuedMessages)
            {
                processGatewayMessage(queuedMessage.Header, queuedMessage.Buffer);
            }
        }

        /// <summary>
        /// Sends a blocking request to the subject specified. 
        /// Returns the reply or null if the request times out.
        /// </summary>
        public Message sendRequest(string subject, Message message, double timeoutSeconds)
        {
            Message resultMessage = null;

            // We create a unique inbox for the reply...
            var inbox = createInbox();

            // We create an auto reset event to block while waiting for the reply...
            var autoResetEvent = new AutoResetEvent(false);

            // We subscribe to the inbox...
            var subscription = subscribe(
                inbox,
                (c, s, rs, m, t) =>
                {
                    // Called on the UV thread when we receive a reply.
                    // We note the result and signal that we have received it.
                    resultMessage = m;
                    autoResetEvent.Set();
                }
            );

            // We note the subscription ID is being used for a sendRequest...
            lock(m_requestSubscriptionIDsLocker)
            {
                m_requestSubscriptionIDs.Add(subscription.SubscriptionID);
            }

            // We create a NetworkMessage to send the request...
            var networkMessage = new NetworkMessage();
            var header = networkMessage.Header;
            header.Action = NetworkMessageHeader.ActionEnum.SEND_MESSAGE;
            header.Subject = subject;
            header.ReplySubject = inbox;
            networkMessage.Message = message;

            // We send the message...
            sendNetworkMessage(networkMessage);

            // We block on the auto reset event, waiting for the result...
            var timeoutMilliseconds = (int)(timeoutSeconds * 1000);
            autoResetEvent.WaitOne(timeoutMilliseconds);

            // We remove the subscription ID from the collection being managed for sendRequests
            // and we dispose the subscription itself...
            lock (m_requestSubscriptionIDsLocker)
            {
                m_requestSubscriptionIDs.Remove(subscription.SubscriptionID);
            }
            subscription.Dispose();

            // We return the result message. This will be null if the request timed out
            // or the request's result if it did not...
            return resultMessage;
        }

        /// <summary>
        /// Unblock the current processMessageQueue() call without waiting for its timeout to elapse.
        /// </summary>
        public void wakeUp()
        {
            m_queuedMessages.wakeUp();
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            // RSSTODO: RELEASE ALL SUBSCRIPTIONS...

            // We send a DISCONNECT message...
            var networkMessage = new NetworkMessage();
            networkMessage.Header.Action = NetworkMessageHeader.ActionEnum.DISCONNECT;
            sendNetworkMessage(networkMessage);

            // We dispose the socket...
            m_clientSocket.Dispose();

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region ClientSocket.ICallback implementation

        /// <summary>
        /// Called when we receive a message from the gateway.
        /// </summary>
        void ClientSocket.ICallback.onDataReceived(ClientSocket socket, Buffer buffer)
        {
            try
            {
                // The buffer holds a serialized NetworkMessage. We deserialize the header and check the action...
                var networkMessage = new NetworkMessage();
                networkMessage.deserializeHeader(buffer);
                var action = networkMessage.Header.Action;
                switch (action)
                {
                    case NetworkMessageHeader.ActionEnum.ACK:
                        onAck();
                        break;

                    case NetworkMessageHeader.ActionEnum.SEND_MESSAGE:
                        onGatewayMessage(networkMessage.Header, buffer);
                        break;
                }
            }
            catch (Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        /// <summary>
        /// Called when our connection to the gateway has disconnected.
        /// </summary>
        void ClientSocket.ICallback.onDisconnected(ClientSocket socket)
        {
            try
            {
                // RSSTODO: DO SOMETHING SENSIBLE HERE!!!
                Logger.info("DISCONNECTED!!!");
            }
            catch (Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        #endregion

        #region Internal methods

        /// <summary>
        /// Releases a subscription.
        /// When all subscriptions for the subject have been released we unsubscribe from the 
        /// subject on the gateway.
        /// </summary>
        internal void releaseSubscription(string subject, Subscription.CallbackInfo callbackInfo)
        {
            bool removeGatewaySubscription = false;
            SubscriptionInfo subscriptionInfo = null;
            lock(m_subscriptionsLocker)
            {
                // We find subsriptions for the subject...
                if (!m_subscriptionsBySubject.TryGetValue(subject, out subscriptionInfo))
                {
                    // There is no subscription for this subject...
                    return;
                }

                // We remove the callback info...
                subscriptionInfo.CallbackInfos.Remove(callbackInfo);

                // If there are no subscriptions left we clean up the subscription and
                // note that we need to unsubscribe on the gateway...
                m_subscriptionsBySubject.Remove(subject);
                m_subscriptionsByID.Remove(subscriptionInfo.SubscriptionID);
                removeGatewaySubscription = true;
            }

            // We remove the gateway subscription if needed...
            if(removeGatewaySubscription)
            {
                // We send an UNSUBSCRIBE message...
                var networkMessage = new NetworkMessage();
                var header = networkMessage.Header;
                header.Action = NetworkMessageHeader.ActionEnum.UNSUBSCRIBE;
                header.SubscriptionID = subscriptionInfo.SubscriptionID;
                sendNetworkMessage(networkMessage);
            }
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Sends a network message to the gateway.
        /// </summary>
        private void sendNetworkMessage(NetworkMessage networkMessage)
        {
            // We serialize the message and send it...
            var buffer = new Buffer();
            networkMessage.serialize(buffer);
            m_clientSocket.write(buffer);
        }

        /// <summary>
        /// Called when we see an ACK message from the Gateway.
        /// </summary>
        private void onAck()
        {
            try
            {
                // We signal that the ACK has been received...
                m_ackSignal.Set();
            }
            catch (Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        /// <summary>
        /// Called when we see a SEND_MESSAGE message from the Gateway, ie when we
        /// receive a message for which we have a subscription.
        /// </summary>
        private void onGatewayMessage(NetworkMessageHeader header, Buffer buffer)
        {
            // We check if we have a response for a sendRequest.
            // If it is, we call back straight away (even if other messages are
            // being dispatched by processMessageQueue.)
            bool isRequestResponse = false;
            lock (m_requestSubscriptionIDsLocker)
            {
                if (m_requestSubscriptionIDs.Contains(header.SubscriptionID))
                {
                    isRequestResponse = true;
                }
            }
            if (isRequestResponse)
            {
                processGatewayMessage(header, buffer);
                return;
            }

            // We check how message dispatch is specified...
            switch (m_connectionParams.MessageDispatch)
            {
                case ConnectionParams.MessageDispatchEnum.CALLBACK_ON_MESSAGING_MESH_THREAD:
                    // We call back to client code straight away on from this thread...
                    processGatewayMessage(header, buffer);
                    break;

                case ConnectionParams.MessageDispatchEnum.PROCESS_MESSAGE_QUEUE:
                    // We queue the message to be dispatched when processMessageQueue() is called.
                    var queuedMessage = new QueuedMessage
                    {
                        Header = header,
                        Buffer = buffer
                    };
                    m_queuedMessages.add(queuedMessage);
                    break;
            }
        }

        /// <summary>
        /// Processes a message from the gateway, calling client callbacks if we have
        /// subscriptions set up for it.
        /// </summary>
        private void processGatewayMessage(NetworkMessageHeader header, Buffer buffer)
        {
            // We check if we have a subscription for the subscription ID...
            List<Subscription.CallbackInfo> callbackInfos = null;
            lock (m_subscriptionsLocker)
            {
                if(m_subscriptionsByID.TryGetValue(header.SubscriptionID, out var subscriptionInfo))
                {
                    callbackInfos = new(subscriptionInfo.CallbackInfos);
                }
            }
            if (callbackInfos != null)
            {
                // We have a subscription so we deserialize the message and call the callbacks...
                var message = new Message();
                message.deserialize(buffer);
                foreach (var callbackInfo in callbackInfos)
                {
                    callbackInfo.Callback(this, header.Subject, header.ReplySubject, message, callbackInfo.Tag);
                }
            }
        }

        /// <summary>
        /// Returns a unique inbox name.
        /// </summary>
        private string createInbox()
        {
            var guid = Guid.NewGuid().ToString("N");
            return $"_INBOX.{guid}";
        }

        #endregion

        #region Private data

        // Connection params...
        private readonly ConnectionParams m_connectionParams;

        // Manages the client socket connecting to the gateway...
        private readonly ClientSocket m_clientSocket;

        // Waits for the ACK signal...
        private AutoResetEvent m_ackSignal = new(false);

        // Info for a subscription to a subject.
        private class SubscriptionInfo
        {
            public string Subject { get; set; } = "";
            public uint SubscriptionID { get; set; } = 0;
            public List<Subscription.CallbackInfo> CallbackInfos { get; set; } = new();
        }

        // Subscriptions keyed by subject and ID, and a locker for them...
        private Dictionary<string, SubscriptionInfo> m_subscriptionsBySubject = new();
        private Dictionary<uint, SubscriptionInfo> m_subscriptionsByID = new();
        private object m_subscriptionsLocker = new();

        // Note 1: Zero is an invalid subscription ID and must note be sent as a subscriptnio ID to the gateway.
        //         The value will be incremented before being used. So subscription IDs will start at one.
        // Note 2: This is accessed via the Interlocked class to ensure thread safety.
        private int m_subscriptionID = 0;

        // Subscription IDs used by blocking sendRequest() and a locker for them...
        private HashSet<uint> m_requestSubscriptionIDs = new();
        private object m_requestSubscriptionIDsLocker = new();

        // Messages received from the gateway, queued for processMessageQueue.
        private class QueuedMessage
        {
            public NetworkMessageHeader Header { get; set; }
            public Buffer Buffer { get; set; }
        }
        private ThreadsafeConsumableList<QueuedMessage> m_queuedMessages = new();

        #endregion
    }
}
