using System;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Header sent with NetworkMessages.
    /// </summary>
    internal class NetworkMessageHeader
    {
        #region Public types

        /// <summary>
        /// Enum for network-message actions.
        /// </summary>
        public enum ActionEnum
        {
            NONE,
            CONNECT,
            DISCONNECT,
            ACK,
            SUBSCRIBE,
            UNSUBSCRIBE,
            SEND_MESSAGE
        };

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets the client ID for a subscription. Helps match subscriptions to callbacks.
        /// </summary>
        public uint SubscriptionID { get; set; } = 0;

        /// <summary>
        /// Gets or sets the subject for subscriptions and message-sending.
        /// Also holds the service-name for CONNECT and DISCONNECT messages.
        /// </summary>
        public string Subject { get; set; } = "";

        /// <summary>
        /// Gets or sets the reply subject for synchronous requests.
        /// </summary>
        public string ReplySubject { get; set; } = "";

        /// <summary>
        /// Gets or sets the action.
        /// </summary>
        public ActionEnum Action { get; set; } = ActionEnum.NONE;

        #endregion

        #region Serialization

        /// <summary>
        /// Serializes the network message header to the current position of the buffer.
        /// </summary>
        public void serialize(Buffer buffer)
        {
            // Subscription ID...
            buffer.write_uint(SubscriptionID);

            // Subject...
            buffer.write_string(Subject);

            // Reply subject...
            buffer.write_string(ReplySubject);

            // Action...
            buffer.write_byte((byte)Action);
        }

        /// <summary>
        /// Deserializes the network message header from the current position in the buffer.
        /// </summary>
        public void deserialize(Buffer buffer)
        {
            // Subscription ID...
            SubscriptionID = buffer.read_uint();

            // Subject...
            Subject = buffer.read_string();

            // Reply subject...
            ReplySubject = buffer.read_string();

            // Action...
            Action = (ActionEnum)buffer.read_byte();
        }

        #endregion
    }
}
