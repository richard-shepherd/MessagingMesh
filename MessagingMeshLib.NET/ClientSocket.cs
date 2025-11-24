using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
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
            void onDataReceived(ClientSocket socket, Buffer buffer);

            /// <summary>
            /// Called when a socket has been disconnected.
            /// Called on the RX thread.
            /// </summary>
            void onDisconnected(ClientSocket socket);
        };

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public ClientSocket(string host, int port, string service)
        {
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

        /// <summary>
        /// Queues a message to be written to the socket.
        /// </summary>
        public void write(Buffer buffer)
        {
            m_writeQueue.add(buffer);
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            // We stop the threads...
            m_stopThreads = true;
            m_rxThread.Join();
            m_txThread.Join();

            // We close the socket...
            m_socket.Close();
            m_socket.Dispose();

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region Private functions

        /// <summary>
        /// Thread entry point for sending data on the socket.
        /// </summary>
        private void threadEntryPointTX()
        {
            try
            {
                Logger.info("Starting TX");
                while (!m_stopThreads)
                {
                    // We wait for data to write to the socket...
                    var buffers = m_writeQueue.waitAndGetItems(millisecondsTimeout: 100);
                    if (buffers.Count == 0)
                    {
                        continue;
                    }

                    // We get an enumerable of all bytes to send, and send the data in fixed size chunks...
                    var bufferIndex = 0;
                    foreach(var b in getBytesToSend(buffers))
                    {
                        // We fill in the buffer...
                        m_sendBuffer[bufferIndex++] = b;
                        if(bufferIndex == SEND_BUFFER_SIZE)
                        {
                            // We have a chunk, so we sent it...
                            send(m_sendBuffer, SEND_BUFFER_SIZE);
                            bufferIndex = 0;
                        }
                    }

                    // We send any remaining data...
                    send(m_sendBuffer, bufferIndex);
                }
            }
            catch(Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        /// <summary>
        /// Sends data to the socket.
        /// </summary>
        private void send(byte[] data, int dataSize)
        {
            int bytesSent = 0;
            while (bytesSent < dataSize)
            {
                bytesSent += m_socket.Send(data, bytesSent, dataSize - bytesSent, SocketFlags.None);
            }
        }

        /// <summary>
        /// Returns an enumberable of all bytes from the buffers.
        /// </summary>
        private IEnumerable<byte> getBytesToSend(ConcurrentQueue<Buffer> buffers)
        {
            foreach(var buffer in buffers)
            {
                var data = buffer.getBuffer();
                var dataSize = buffer.getBufferSize();
                for(var i=0; i< dataSize; ++i)
                {
                    yield return data[i];
                }
            }
        }

        /// <summary>
        /// Thread entry point for receiving data on the socket.
        /// </summary>
        private void threadEntryPointRX()
        {
            try
            {
                Logger.info("Starting RX");
                var buffer = new byte[8192];  // RSSTODO: CHANGE THIS. MAYBE ALL OF THIS!!!
                while (!m_stopThreads)
                {
                    int bytesReceived = m_socket.Receive(buffer);
                    onDataReceived(buffer, bytesReceived);
                }
            }
            catch (Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        /// <summary>
        /// Called when data has been received on a socket.
        /// </summary>
        private void onDataReceived(byte[] buffer, int bufferSize)
        {
            try
            {
                // We check for errors...
                if (bufferSize < 1)
                {
                    return;
                }

                // All data we receive comes in the form of network-data messages.
                // These look like:
                // - size  (int32, little-endian)
                // - bytes[size]
                //
                // There are different cases where we can get this callback:
                // 
                // 1. A data buffer containing all (and only) the data for one message.
                //
                // 2. A data buffer containing multiple messages.
                // 
                // 3. Partial data for a message. In particular this will be the case
                //    for very large messages which do not fit into one data buffer.
                //
                // It could be that the buffer holds data for some combination of the
                // types described above. For example:
                // - Two complete messages followed by one partial message.
                // - The remainder of a message from a previous update, plus one or more new messages.
                // - Other permutations and combinations along these lines.
                // 
                // In all cases we only really need to know two things:
                // 
                // a) Are we expecting a new message? 
                //    If so, we read the size and then read that number of bytes.
                // 
                // b) Are we in the process of reading a message?
                //    We have a message that started in a previous update and has data
                //    continuing into this one.
                // 
                // We are always reading one message at a time into the m_currentMessage object.
                // This is a shared pointer to a NetworkData object.
                // - If this is null we are expecting a new message and need to read the size. 
                // - If this is not null we are reading additional data for an existing message.
                // 
                // The NetworkData object knows the size we are expecting, so we can continue to
                // read data until we have all the data we need for the message.
                // 
                // NOTE: We need to be careful when reading the size for a new message. It is
                //       possible that the size itself may only be received across multiple of
                //       these callbacks.

                // We read the buffer...
                int bufferPosition = 0;
                while (bufferPosition < bufferSize)
                {
                    // If we do not have a current message we create one...
                    if (m_currentMessage == null)
                    {
                        m_currentMessage = new Buffer();
                    }

                    // We read data into the current message...
                    int bytesRead = m_currentMessage.readNetworkMessage(buffer, bufferSize, bufferPosition);

                    // If we have read all data for the current message we call back with it...
                    if (m_currentMessage.HasAllData)
                    {
                        // We reset the position of the message / buffer so that it is 
                        // ready to be read by the client in the callback...
                        m_currentMessage.resetPosition();
                        m_callback?.onDataReceived(this, m_currentMessage);

                        // We clear the current message to start a new one...
                        m_currentMessage = null;
                    }

                    // We update the buffer position and loop to check if there is
                    // more data to read...
                    bufferPosition += bytesRead;
                }
            }
            catch (Exception ex)
            {
                Logger.error(ex.Message);
            }
        }

        #endregion

        #region Private data

        // Callback...
        private ICallback m_callback = null;

        // The socket connection to the gateway...
        private readonly Socket m_socket;

        // RX and TX threads...
        private readonly Thread m_rxThread;
        private readonly Thread m_txThread;

        // Used to signal threads to stop...
        private volatile bool m_stopThreads = false;

        // Messages queued for writing...
        private readonly ThreadsafeConsumableQueue<Buffer> m_writeQueue = new();

        // A buffer for the message being actively read from the socket...
        private Buffer m_currentMessage = null;

        // Fixed sized buffer for sending data to the socket...
        private const int SEND_BUFFER_SIZE = 8192;
        private byte[] m_sendBuffer = new byte[SEND_BUFFER_SIZE];

        #endregion
    }
}
