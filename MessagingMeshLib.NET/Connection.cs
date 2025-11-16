using System;
using System.Net.Sockets;
using System.Runtime.Remoting.Messaging;
using System.Xml.Linq;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Manages a client connection to the messaging mesh.
    /// </summary>
    public class Connection
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Connection(ConnectionParams connectionParams)
        {
            // We create the socket connection to the gateway...
            m_clientSocket = new ClientSocket(connectionParams.GatewayHost, connectionParams.GatewayPort, connectionParams.Service);

            // We send a CONNECT message...
            var networkMessage = new NetworkMessage();
            networkMessage.Header.Action = NetworkMessageHeader.ActionEnum.CONNECT;
            networkMessage.Header.Subject = connectionParams.Service;
            sendNetworkMessage(networkMessage);
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

        #region Private functions

        /// <summary>
        /// Sends a network-message to the gateway.
        /// </summary>
        private void sendNetworkMessage(NetworkMessage networkMessage)
        {
            // We serialize the message and send it...
            var buffer = new Buffer();
            networkMessage.serialize(buffer);
            m_clientSocket.write(buffer);
        }

        #endregion

        #region Private data

        // Manages the client socket connecting to the gateway...
        private readonly ClientSocket m_clientSocket;

        #endregion
    }
}
