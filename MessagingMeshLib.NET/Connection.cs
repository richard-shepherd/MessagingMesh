using System;
using System.Runtime.Remoting.Messaging;
using System.Threading;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Manages a client connection to the messaging mesh.
    /// </summary>
    public class Connection : ClientSocket.ICallback
    {
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

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

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
        /// Called when we see the ACK message from the Gateway.
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
        /// Called when we see the SEND_MESSAGE message from the Gateway.
        /// </summary>
        private void onSendMessage(NetworkMessageHeader header, Buffer buffer)
        {
            // RSSTODO: WRITE THIS!!!
        }

        #endregion

        #region Private data

        // Manages the client socket connecting to the gateway...
        private readonly ClientSocket m_clientSocket;

        // Waits for the ACK signal...
        private AutoResetEvent m_ackSignal = new(false);

        #endregion
    }
}
