using MessagingMeshLib.NET;
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

        #region Properties

        /// <summary>
        /// Gets whether we hold all data from a network message update.
        /// </summary>
        public bool HasAllData => m_hasAllData;

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Buffer()
        {
        }

        /// <summary>
        /// Constructor.
        /// </summary>
        public Buffer(int initialSize)
        {
            m_buffer = new byte[initialSize];
            m_bufferSize = initialSize;
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

        /// <summary>
        /// Reads data from a network data buffer until we have all the data for
        /// the buffer as specified by the size in the network message.
        /// Returns the number of bytes read from the buffer.
        /// </summary>
        public int readNetworkMessage(byte[] buffer, int bufferSize, int bufferPosition)
        {
            // See also the comments in Socket::onDataReceived (in the C++ library)
            // about how data for a message can be received across multiple updates.

            // If we already have all the data we need, there is nothing to do...
            if (m_hasAllData)
            {
                return 0;
            }

            // We make sure that we have the message size.
            // (This is a no-op if we already know the size.)
            int bytesRead = readNetworkMessageSize(buffer, bufferSize, bufferPosition);
            if (m_bufferSize == 0)
            {
                // We do not (yet) have the size...
                return bytesRead;
            }
            bufferPosition += bytesRead;

            // We find the number of bytes we need. This may not be the same as
            // the size of the message, as we may have already read from of the
            // data from previous updates.
            int sizeRequired = m_bufferSize - m_position;

            // We find how much data there is available in the buffer and
            // work out how many bytes to read from the buffer...
            int sizeAvailable = bufferSize - bufferPosition;
            int sizeToRead = Math.Min(sizeRequired, sizeAvailable);

            // We copy the data into our buffer...
            System.Buffer.BlockCopy(buffer, bufferPosition, m_buffer, m_position, sizeToRead);
            bytesRead += sizeToRead;

            // We update the data buffer position. It may be that we still have not read the
            // whole message and we could need this position so we know where to append data
            // from future updates...
            m_position += sizeToRead;

            // We check if we have the whole message...
            if (m_position == m_bufferSize)
            {
                m_hasAllData = true;
            }

            return bytesRead;
        }

        #endregion

        #region read() and write() methods for various types

        /// <summary>
        /// Reads a byte from the buffer.
        /// </summary>
        public byte read_byte()
        {
            var size = sizeof(byte);
            checkBufferSize_Read(size);
            var result = m_buffer[m_position];
            updatePosition_Read(size);
            return result;
        }

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
        /// Reads a signed int32 from the buffer.
        /// </summary>
        public int read_int32()
        {
            var size = sizeof(int);
            checkBufferSize_Read(size);
            var result = BitConverter.ToInt32(m_buffer, m_position);
            updatePosition_Read(size);
            return result;
        }

        /// <summary>
        /// Writes a signed int32 to the buffer.
        /// </summary>
        public void write_int32(int item)
        {
            var size = sizeof(int);
            checkBufferSize_Write(size);
            m_buffer[m_position + 0] = (byte)item;
            m_buffer[m_position + 1] = (byte)(item >> 8);
            m_buffer[m_position + 2] = (byte)(item >> 16);
            m_buffer[m_position + 3] = (byte)(item >> 24);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Reads an unsigned int from the buffer.
        /// </summary>
        public uint read_uint32()
        {
            var size = sizeof(uint);
            checkBufferSize_Read(size);
            var result = BitConverter.ToUInt32(m_buffer, m_position);
            updatePosition_Read(size);
            return result;
        }

        /// <summary>
        /// Writes an unsigned int to the buffer.
        /// </summary>
        public void write_uint32(uint item)
        {
            var size = sizeof(uint);
            checkBufferSize_Write(size);
            m_buffer[m_position + 0] = (byte)item;
            m_buffer[m_position + 1] = (byte)(item >> 8);
            m_buffer[m_position + 2] = (byte)(item >> 16);
            m_buffer[m_position + 3] = (byte)(item >> 24);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Reads a signed int64 from the buffer.
        /// </summary>
        public long read_int64()
        {
            var size = sizeof(long);
            checkBufferSize_Read(size);
            var result = BitConverter.ToInt64(m_buffer, m_position);
            updatePosition_Read(size);
            return result;
        }

        /// <summary>
        /// Writes a signed int64 to the buffer.
        /// </summary>
        public void write_int64(long item)
        {
            var size = sizeof(long);
            checkBufferSize_Write(size);
            m_buffer[m_position + 0] = (byte)item;
            m_buffer[m_position + 1] = (byte)(item >> 8);
            m_buffer[m_position + 2] = (byte)(item >> 16);
            m_buffer[m_position + 3] = (byte)(item >> 24);
            m_buffer[m_position + 4] = (byte)(item >> 32);
            m_buffer[m_position + 5] = (byte)(item >> 40);
            m_buffer[m_position + 6] = (byte)(item >> 48);
            m_buffer[m_position + 7] = (byte)(item >> 56);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Reads an unsigned int64 from the buffer.
        /// </summary>
        public ulong read_uint64()
        {
            var size = sizeof(ulong);
            checkBufferSize_Read(size);
            var result = BitConverter.ToUInt64(m_buffer, m_position);
            updatePosition_Read(size);
            return result;
        }

        /// <summary>
        /// Writes an unsigned int64 to the buffer.
        /// </summary>
        public void write_uint64(ulong item)
        {
            var size = sizeof(ulong);
            checkBufferSize_Write(size);
            m_buffer[m_position + 0] = (byte)item;
            m_buffer[m_position + 1] = (byte)(item >> 8);
            m_buffer[m_position + 2] = (byte)(item >> 16);
            m_buffer[m_position + 3] = (byte)(item >> 24);
            m_buffer[m_position + 4] = (byte)(item >> 32);
            m_buffer[m_position + 5] = (byte)(item >> 40);
            m_buffer[m_position + 6] = (byte)(item >> 48);
            m_buffer[m_position + 7] = (byte)(item >> 56);
            updatePosition_Write(size);
        }

        /// <summary>
        /// Reads a double from the buffer.
        /// </summary>
        public double read_double()
        {
            var size = sizeof(double);
            checkBufferSize_Read(size);
            var result = BitConverter.ToDouble(m_buffer, m_position);
            updatePosition_Read(size);
            return result;
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
        /// Reads a string from the buffer.
        /// </summary>
        public string read_string()
        {
            // Strings are serialized as [length][chars].

            // We read the length...
            var length = read_int32();

            // We check the buffer size...
            checkBufferSize_Read(length);

            // We read the string...
            var result = Encoding.UTF8.GetString(m_buffer, m_position, length);

            // We update the position...
            updatePosition_Read(length);

            return result;
        }

        /// <summary>
        /// Writes a string to the buffer.
        /// </summary>
        public void write_string(string item)
        {
            // Strings are serialized as [length][chars].

            // We convert the string to UTF-8...
            var utf8Bytes = Encoding.UTF8.GetBytes(item);

            // We write the length...
            write_int32(utf8Bytes.Length);

            // We write the characters...
            write_bytes(utf8Bytes);
        }

        /// <summary>
        /// Reads bytes from the buffer.
        /// </summary>
        public byte[] read_bytes(int length)
        {
            // We check the buffer size...
            checkBufferSize_Read(length);

            // We create a byte array of the right size and copy dat ainto it from the buffer...
            var result = new byte[length];
            System.Buffer.BlockCopy(m_buffer, m_position, result, 0, length);

            // We update the position...
            updatePosition_Read(length);

            return result;
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
        /// Reads a field from the buffer.
        /// </summary>
        public Field read_field()
        {
            // We create a new field and deserialize into it...
            var field = new Field();
            field.deserialize(this);
            return field;
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
        /// Reads a message from the buffer.
        /// </summary>
        public Message read_message()
        {
            // We create a new message and deserialize into it...
            var message = new Message();
            message.deserialize(this);
            return message;
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

        /// <summary>
        /// Reads a bool from the buffer.
        /// </summary>
        public bool read_bool()
        {
            var b = read_byte();
            return b == 1;
        }

        /// <summary>
        /// Writes a bool to the buffer.
        /// </summary>
        public void write_bool(bool item)
        {
            var b = item ? (byte)1 : (byte)0;
            write_byte(b);
        }

        /// <summary>
        /// Reads a BLOB from the buffer.
        /// </summary>
        public byte[] read_blob()
        {
            // We read the length...
            var length = read_int32();

            // We read the bytes...
            return read_bytes(length);
        }

        /// <summary>
        /// Writes a BLOB to the buffer.
        /// </summary>
        public void write_blob(byte[] item)
        {
            // We write the length...
            write_int32(item.Length);

            // We write the bytes...
            write_bytes(item);
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Reads the network message size (or as much as can be read) from the buffer.
        /// </summary>
        private int readNetworkMessageSize(byte[] buffer, int bufferSize, int bufferPosition)
        {
            // We check if we already have the size...
            if (m_bufferSize != 0)
            {
                return 0;
            }

            // We read as many bytes as we can for the size from the buffer...
            int bytesRead = 0;
            while (m_networkMessageSizeBufferPosition < SIZE_SIZE)
            {
                if (bufferPosition >= bufferSize) break;
                m_networkMessageSizeBuffer[m_networkMessageSizeBufferPosition++] = buffer[bufferPosition++];
                bytesRead++;
            }

            // If we have read all the bytes for the size, we get the size...
            if (m_networkMessageSizeBufferPosition == SIZE_SIZE)
            {
                // We copy the size buffer to the m_bufferSize field. (We can do this as
                // the messaging-mesh network protocol for int32 is little-endian.)
                m_bufferSize = BitConverter.ToInt32(m_networkMessageSizeBuffer, 0);

                // We allocate the data buffer for the size...
                m_dataSize = m_bufferSize;
                m_buffer = new byte[m_bufferSize];

                // We make sure that the position is four bytes from the start of the buffer.
                // The first four bytes are reserved for the size itself. The data will be
                // written after this.
                m_position = SIZE_SIZE;
            }

            // We return the number of bytes read from the buffer (which may have not 
            // been enough to fully parse the size)...
            return bytesRead;
        }

        /// <summary>
        /// Checks that the buffer is large enough to read the specified number of bytes.
        /// Throws a MessagingMeshException if the buffer is not large enough.
        /// </summary>
        private void checkBufferSize_Read(int bytesRequired)
        {
            if (m_position + bytesRequired > m_dataSize)
            {
                // The buffer is not large enough to read the bytes requested...
                throw new MessagingMeshException("Buffer is not large enough to read requested data");
            }
        }

        /// <summary>
        /// Checks that the buffer has the capacity to hold the number of bytes specified
        /// and expands it if it does not.
        /// Throws a MessagingMeshException if the buffer required is too large.
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
            Logger.warn("RSSTODO: EXPAND BUFFER!!!!!");

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
        /// Updates the position to reflect bytes read from the buffer.
        /// </summary>
        private void updatePosition_Read(int bytesRead)
        {
            // We update the position...
            m_position += bytesRead;
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
        private const int INITIAL_SIZE = 1024;

        // The buffer...
        private byte[] m_buffer = null;
        private int m_bufferSize = 0;

        // The current position at which data will be written.
        // This starts after the bytes reserved for the size.
        private int m_position = SIZE_SIZE;

        // The size of all data written to the buffer.
        // Note: This includes the size held in the first four bytes.
        private int m_dataSize = SIZE_SIZE;

        // True if we have all data for a network message, false if not.
        private bool m_hasAllData = false;

        // Buffer when reading the size from a network message.
        // (We may receive the size across multiple network updates.)
        private byte[] m_networkMessageSizeBuffer = new byte[SIZE_SIZE];
        private int m_networkMessageSizeBufferPosition = 0;

        #endregion
    }
}
