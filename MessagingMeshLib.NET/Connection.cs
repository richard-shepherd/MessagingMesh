using System;
using System.Collections.Generic;
using System.Runtime.Remoting.Messaging;
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
            var subscriptionCallbackInfo = new Subscription.CallbackInfo
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
                subscriptionInfo.SubscriptionCallbackInfos.Add(subscriptionCallbackInfo);
            }

            // We subscribe to the gateway if needed... 
            if(makeGatewaySubscription)
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
            return new Subscription(this, subject, subscriptionCallbackInfo);
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
                        onSendMessage(networkMessage.Header, buffer);
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
                subscriptionInfo.SubscriptionCallbackInfos.Remove(callbackInfo);

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
        private void onSendMessage(NetworkMessageHeader header, Buffer buffer)
        {
            try
            {
                // We check if we have a subscription for the subscription ID...
                SubscriptionInfo subscriptionInfo = null;
                lock(m_subscriptionsLocker)
                {
                    m_subscriptionsByID.TryGetValue(header.SubscriptionID, out subscriptionInfo);
                }
                if(subscriptionInfo != null)
                {
                    // We have a subscription so we deserialize the message and call the callbacks...
                    var message = new Message();
                    message.deserialize(buffer);
                    foreach(var callbackInfo in subscriptionInfo.SubscriptionCallbackInfos)
                    {
                        callbackInfo.Callback(this, header.Subject, header.ReplySubject, message, callbackInfo.Tag);
                    }
                }
            }
            catch(Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        #endregion

        #region Private data

        // Manages the client socket connecting to the gateway...
        private readonly ClientSocket m_clientSocket;

        // Waits for the ACK signal...
        private AutoResetEvent m_ackSignal = new(false);

        // Info for a subscription to a subject.
        private class SubscriptionInfo
        {
            public string Subject { get; set; } = "";
            public uint SubscriptionID { get; set; } = 0;
            public List<Subscription.CallbackInfo> SubscriptionCallbackInfos { get; set; } = new();
        }

        // Subscriptions keyed by subject and ID, and a locker for them...
        private Dictionary<string, SubscriptionInfo> m_subscriptionsBySubject = new();
        private Dictionary<uint, SubscriptionInfo> m_subscriptionsByID = new();
        private object m_subscriptionsLocker = new();

        // Note 1: Zero is an invalid subscription ID and must note be sent as a subscriptnio ID to the gateway.
        //         The value will be incremented before being used. So subscription IDs will start at one.
        // Note 2: This is accessed via the Interlocked class to ensure thread safety.
        private int m_subscriptionID = 0;

        #endregion
    }
}
