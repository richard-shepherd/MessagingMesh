using System;
using System.Runtime.Remoting.Messaging;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Message sent between messaging-mesh clients and gateways for
    /// updates and events. Includes a header indicating the type of
    /// update being sent, plus a message payload.
    /// </summary>
    internal class NetworkMessage
    {
        #region Properties

        /// <summary>
        /// Gets or sets the header.
        /// </summary>
        public NetworkMessageHeader Header { get; set; } = new();

        /// <summary>
        /// Message payload.
        /// </summary>
        public Message Message { get; set; } = new();

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public NetworkMessage()
        {
        }

        #endregion

        #region Serialization

        /// <summary>
        /// Serializes the network message to the current position of the buffer.
        /// </summary>
        public void serialize(Buffer buffer)
        {
            // Header...
            Header.serialize(buffer);

            // Message.
            Message.serialize(buffer);
        }

        /// <summary>
        /// Deserializes the network message from the current position in the buffer.
        /// </summary>
        public void deserialize(Buffer buffer)
        {
            deserializeHeader(buffer);
            deserializeMessage(buffer);
        }

        /// <summary>
        /// Deserializes the header from the current position in the buffer.
        /// </summary>
        public void deserializeHeader(Buffer buffer)
        {
            Header.deserialize(buffer);
        }

        /// <summary>
        /// Deserializes the message from the current position in the buffer.
        /// </summary>
        public void deserializeMessage(Buffer buffer)
        {
            Message.deserialize(buffer);
        }

        #endregion
    }
}
