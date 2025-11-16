using System;
using System.Diagnostics;
using System.Net.Sockets;
using System.Threading;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Manages a client socket connecting to a messaging mesh gateway.
    /// </summary><remarks>
    /// Threads
    /// -------
    /// We have two threads:
    /// - [SERVICE]-RX: Listens for and processes data from the gateway.
    /// - [SERVICE]-TX: Sends data to the gateway.
    /// </remarks>
    internal class ClientSocket : IDisposable
    {
        #region ICallback definition

        /// <summary>
        /// Interface for socket callbacks.
        /// </summary>
        public interface ICallback
        {
            /// <summary>
            /// Called when data has been received on the socket.
            /// </summary>
            void onDataReceived(Socket socket, Buffer buffer);

            /// <summary>
            /// Called when a socket has been disconnected.
            /// Called on the RX thread.
            /// </summary>
            void onDisconnected(Socket socket);
        };

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public ClientSocket(string host, int port, string service)
        {
            m_host = host;
            m_port = port;
            m_service = service;

            // We create the socket connection to the gateway...
            Logger.info($"Creating gateway connection to {host}:{port} for service {service}");
            m_socket = new Socket(SocketType.Stream, ProtocolType.Tcp);
            m_socket.NoDelay = true;  // Turn off Nagle's algorithm
            m_socket.Connect(host, port);

            // We create the RX thread...
            m_rxThread = new Thread(threadEntryPointRX);
            m_rxThread.Name = $"MM-{service}-RX";
            m_rxThread.IsBackground = true;
            m_rxThread.Start();

            // We create the TX thread...
            m_txThread = new Thread(threadEntryPointTX);
            m_txThread.Name = $"MM-{service}-TX";
            m_rxThread.IsBackground = true;
            m_txThread.Start();
        }

        /// <summary>
        /// Sets the object into which we call back when we get updates on the socket.
        /// </summary>
        public void setCallback(ICallback callback)
        {
            m_callback = callback;
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region Private functions

        private void threadEntryPointTX()
        {
            Thread.Sleep(5000);
        }

        private void threadEntryPointRX()
        {
            Thread.Sleep(5000);
        }

        #endregion

        #region Private data

        // The gateway hostname or IP address...
        private readonly string m_host;

        // The gateway port...
        private readonly int m_port;

        // The service name...
        private readonly string m_service;

        // Callback...
        private ICallback m_callback = null;

        // The socket connection to the gateway...
        private readonly Socket m_socket;

        // RX and TX threads...
        private readonly Thread m_rxThread;
        private readonly Thread m_txThread;

        #endregion
    }
}
