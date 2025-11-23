namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Parameters requesting a connection to the messaging mesh.
    /// Passed to the constructor of the Connection class.
    /// </summary>
    public class ConnectionParams
    {
        #region Public types

        /// <summary>
        /// Enum for how messages are dispatched to clients.
        /// </summary>
        public enum MessageDispatchEnum
        {
            /// <summary>
            /// Subscription callbacks are called on the Connection's messaging thread.
            /// </summary>
            CALLBACK_ON_MESSAGING_MESH_THREAD,

            /// <summary>
            /// Subscription callbacks are manually dispatched on the client thread by calling Connection.processMessageQueue().
            /// </summary>
            PROCESS_MESSAGE_QUEUE
        }

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets the gateway hostname or IP address.
        /// </summary>
        public string GatewayHost { get; set; } = "";

        /// <summary>
        /// Gets or sets the gateway port.
        /// </summary>
        public int GatewayPort { get; set; } = 0;

        /// <summary>
        /// Gets or sets the messaging-mesh service to join.
        /// </summary>
        public string Service { get; set; } = "";

        /// <summary>
        /// Gets or sets how subscribed messages are dispatched to client code.
        /// </summary>
        public MessageDispatchEnum MessageDispatch { get; set; } = MessageDispatchEnum.PROCESS_MESSAGE_QUEUE;

        #endregion
    }
}
