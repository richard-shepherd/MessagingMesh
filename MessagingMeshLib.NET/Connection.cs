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
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            // We dispose the socket...
            m_clientSocket.Dispose();

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region Private data

        // Manages the client socket connecting to the gateway...
        private readonly ClientSocket m_clientSocket;

        #endregion
    }
}
