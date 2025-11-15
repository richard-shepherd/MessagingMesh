using System;
using System.Linq;
using System.Text;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// A binary buffer managing a byte-array, holding for example the network 
    /// serialization of a NetworkMessage.
    ///
    /// Two ways the Buffer is used
    /// ---------------------------
    /// - Serialize data into the buffer and then send it over the network.
    /// - Receive a network message into a Buffer and then deserialize data from it.
    /// 
    /// These are discussed in more detail below.
    /// 
    /// Serialize data into the buffer
    /// ------------------------------
    /// The buffer starts at an initial size which will be expanded if data 
    /// is written to the buffer that would go beyond that size.
    ///
    /// Data is written to the buffer at the current position. By default this
    /// will be updated to the next available position as data is written to the 
    /// buffer. You can change the position manually if you need to write into 
    /// the buffer at a point other than the end.
    ///
    /// You can request the data size from the buffer which will return the number 
    /// of bytes in the buffer, regardless of where the position is set.
    /// 
    /// Note on data-size vs buffer-size:
    /// - The data-size is the size of all data serialized to the Buffer.
    /// - The buffer-size is the size of the byte-array itself, which may not be
    ///   fully filled by the serialized data.
    ///
    /// After data has been serialized into the Buffer you can pass it to the
    /// Socket::write() method to send the data via the socket.
    /// 
    /// Receive a network message
    /// -------------------------
    /// When a Socket receives data over the network it creates a new Buffer object
    /// and populates it with the data it receives. 
    /// 
    /// The first bytes in a network message are the size of the message / Buffer so
    /// once these have been received the Socket and Buffer can work together to 
    /// populate the Buffer. For large messages this may be done over multiple network
    /// updates received by the Socket. 
    /// 
    /// The byte-array includes the size
    /// --------------------------------
    /// The byte-array managed by the Buffer includes the size of the byte-array as 
    /// a little-endian int32 serialized into the first four bytes of the array.
    /// 
    /// Note: This size includes the size of the size itself.
    /// 
    /// For example, a buffer holding a double and an int will look like:
    /// - size   (4 bytes), value=16
    /// - double (8 bytes)
    /// - int    (4 bytes)
    /// 
    /// The total size of the buffer - including the size itself - is 16 bytes, and 
    /// this is stored as the size in the first four bytes of the array.
    /// 
    /// 
    /// 
    /// The buffer includes the size
    /// ----------------------------
    /// The first four bytes of the buffer is the data size stored in a little-endian
    /// format. This helps when sending buffers over the network as receiving code can
    /// see how much data to expect.
    /// 
    /// NOTE: This is the size of the data itself, not including the size.
    ///       So it is m_dataSize - INITIAL_POSITION.
    /// 
    /// The size is added to the buffer when the getBuffer() method is called.
    /// </summary>
    internal class Buffer
    {
        #region Public constants

        /// <summary>
        /// The size in bytes of the buffer size (int32) - which we store at the start of the buffer.
        /// </summary>
        public const int SIZE_SIZE = 4;

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Buffer()
        {
        }

        /// <summary>
        /// Gets the buffer.
        /// </summary>
        public byte[] getBuffer()
        {
            // If the buffer has not yet been allocated we allocate a buffer to hold the
            // size, as client code is always expecting a buffer with at least a size at
            // the start of the buffer...
            if (m_buffer == null)
            {
                m_buffer = new byte[SIZE_SIZE];
                m_bufferSize = SIZE_SIZE;
            }

            // We update the data size, stored in the first four bytes of the buffer...
            var bytes = BitConverter.GetBytes(m_dataSize);
            System.Buffer.BlockCopy(bytes, 0, m_buffer, 0, SIZE_SIZE);

            // We return the buffer...
            return m_buffer;
        }

        /// <summary>
        /// Gets the size of the data stored in the buffer.
        /// This includes the four bytes for the size plus the data.
        /// </summary>
        public int getBufferSize()
        { 
            return m_dataSize; 
        }

        /// <summary>
        /// Resets the position to the initial position for reading data.
        /// Note: This is the position after the size.
        /// </summary>
        public void resetPosition() 
        { 
            m_position = SIZE_SIZE; 
        }

        /// <summary>
        /// Gets the position in the buffer where data will be written.
        /// </summary>
        public int getPosition() 
        {
            return m_position;
        }

        /// <summary>
        /// Sets the position in the buffer where data will be written.
        /// </summary>
        public void setPosition(int position)
        {
            m_position = position;
        }

        #endregion

        #region write() methods for various types

        /// <summary>
        /// Writes a byte to the buffer.
        /// </summary>
        public void write_byte(byte item)
        {
            var size = sizeof(byte);
            checkBufferSize_Write(size);
            m_buffer[m_position] = item;
            updatePosition_Write(size);
        }

        /// <summary>
        /// Writes a signed int to the buffer.
        /// </summary>
        public void write_int(int item)
        {
            var size = sizeof(int);
            checkBufferSize_Write(size);
            var bytes = BitConverter.GetBytes(item);
            System.Buffer.BlockCopy(bytes, 0, m_buffer, m_position, size);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Writes an unsigned int to the buffer.
        /// </summary>
        public void write_uint(uint item)
        {
            var size = sizeof(uint);
            checkBufferSize_Write(size);
            var bytes = BitConverter.GetBytes(item);
            System.Buffer.BlockCopy(bytes, 0, m_buffer, m_position, size);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Writes a double to the buffer.
        /// </summary>
        public void write_double(double item)
        {
            var size = sizeof(double);
            checkBufferSize_Write(size);
            var bytes = BitConverter.GetBytes(item);
            System.Buffer.BlockCopy(bytes, 0, m_buffer, m_position, size);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Writes a string to the buffer.
        /// </summary>
        public void write_string(string item)
        {
            // String are serialized as [length][chars].

            // We convert the string to UTF-8...
            var utf8Bytes = Encoding.UTF8.GetBytes(item);

            // We write the length...
            write_int(utf8Bytes.Length);

            // We write the characters...
            write_bytes(utf8Bytes);
        }

        /// <summary>
        /// Writes bytes to the buffer from the array passed in.
        /// </summary>
        public void write_bytes(byte[] bytes)
        {
            // We make sure that the buffer can hold the new data...
            var size = bytes.Length;
            checkBufferSize_Write(size);

            // We write the data to the buffer...
            System.Buffer.BlockCopy(bytes, 0, m_buffer, m_position, size);

            // We update the position and data size... 
            updatePosition_Write(size);
        }

        /// <summary>
        /// Writes a field to the buffer.
        /// </summary>
        public void write_field(Field item)
        {
            // We call the field's serialize() method. This calls back into the buffer
            // to write the data for the field and the specific type it is managing...
            item.serialize(this);
        }

        /// <summary>
        /// Writes a message to the buffer.
        /// </summary>
        public void write_message(Message item)
        {
            // We call the message's serialize() method. This calls back into the buffer
            // to write the data for the message and the fields it is managing...
            item.serialize(this);
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Checks that the buffer has the capacity to hold the number of bytes specified
        /// and expands it if it does not.
        /// Throws a MessagingMesh::Exception if the buffer required is too large.
        /// </summary>
        private void checkBufferSize_Write(int bytesRequired)
        {
            // We check if we can fit the bytes-required into the buffer at the current position...
            if (m_position + bytesRequired <= m_bufferSize)
            {
                // The bytes will fit...
                return;
            }

            // The bytes will not fit, so we expand the buffer...
            expandBuffer();

            // We re-check that the bytes-required will fit with the new size...
            checkBufferSize_Write(bytesRequired);
        }

        /// <summary>
        /// Expands the buffer by doubling its size.
        /// </summary>
        private void expandBuffer()
        {
            if (m_bufferSize >= Int32.MaxValue)
            {
                throw new MessagingMeshException("Buffer is at max capacity");
            }

            // If the buffer has not yet been allocated, we allocate the initial size...
            if (m_buffer == null)
            {
                m_buffer = new byte[INITIAL_SIZE];
                m_bufferSize = INITIAL_SIZE;
                return;
            }

            // We create a new buffer double the size and copy the existing data into it...
            var newBufferSize = m_bufferSize * 2;
            var newBuffer = new byte[newBufferSize];
            System.Buffer.BlockCopy(m_buffer, 0, newBuffer, 0, m_bufferSize);

            // We use the new buffer...
            m_buffer = newBuffer;
            m_bufferSize = newBufferSize;
        }

        /// <summary>
        /// Updates the position and data-size to reflect bytes written to the buffer.
        /// </summary>
        private void updatePosition_Write(int bytesWritten)
        {
            // We update the position...
            m_position += bytesWritten;

            // We update the data-size if the new position is more than the previous size.
            // (This may not be the case if the position has been manually changed to write
            // to an earlier point in the buffer.)
            if (m_position > m_dataSize)
            {
                m_dataSize = m_position;
            }
        }

        #endregion

        #region Private data

        // The initial size we allocate for the buffer...
        private const int INITIAL_SIZE = 8192;

        // The buffer...
        private byte[] m_buffer = null;
        private int m_bufferSize = 0;

        // The current position at which data will be written.
        // This starts after the bytes reserved for the size.
        private int m_position = SIZE_SIZE;

        // The size of all data written to the buffer.
        // Note: This includes the size held in the first four bytes.
        private int m_dataSize = SIZE_SIZE;

        #endregion
    }
}
