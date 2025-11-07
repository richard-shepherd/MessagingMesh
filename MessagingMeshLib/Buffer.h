#pragma once
#include <memory>
#include <vector>
#include <string>
#include "SharedAliases.h"

namespace MessagingMesh
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
    class Buffer
    {
    // Public constants...
    public:
        // The size in bytes of the buffer size (int32) - which we store at the start of the buffer...
        static const int SIZE_SIZE = 4;

    // Public methods...
    public:
        // Creates a Buffer instance.
        static BufferPtr create() { return BufferPtr(new Buffer()); }

        // Destructor.
        ~Buffer();

        // Gets the buffer.
        char* getBuffer();

        // Gets the size of the data stored in the buffer.
        // This includes the four bytes for the size plus the data.
        int32_t getBufferSize() { return m_dataSize; }

        // Resets the position to the initial position for reading data.
        // Note: This is the position after the size.
        void resetPosition() { m_position = SIZE_SIZE; }

        // Gets the position in the buffer where data will be written.
        int32_t getPosition() const { return m_position; }

        // Sets the position in the buffer where data will be written.
        void setPosition(int32_t position) { m_position = position; }

        // Returns true if we hold all data for a network message, false if not.
        bool hasAllData() const { return m_hasAllData; }

        // Reads data from a network data buffer until we have all the data for
        // the buffer as specified by the size in the network message.
        // Returns the number of bytes read from the buffer.
        size_t readNetworkMessage(const char* pBuffer, size_t bufferSize, size_t bufferPosition);

    // write() method for various types...
    public:
        // Writes an int8 to the buffer.
        void write_int8(int8_t item);

        // Writes a signed int32 to the buffer.
        void write_int32(int32_t item);

        // Writes an unsigned int32 to the buffer.
        void write_uint32(uint32_t item);

        // Writes a double to the buffer.
        void write_double(double item);

        // Writes a string to the buffer.
        void write_string(const std::string& item);

        // Writes bytes to the buffer from the pointer passed in.
        void write_bytes(const void* p, int32_t size);

        // Writes a field to the buffer.
        void write_field(const ConstFieldPtr& item);

        // Writes a message to the buffer.
        void write_message(const ConstMessagePtr& item);

    // read() method for various types...
    public:
        // Reads an int8 from the buffer.
        int8_t read_int8();

        // Reads an int32 from the buffer.
        int32_t read_int32();

        // Reads a uint32 from the buffer.
        uint32_t read_uint32();

        // Reads a double from the buffer.
        double read_double();

        // Reads a string from the buffer.
        std::string read_string();

        // Reads bytes from the buffer to the pointer passed in.
        // NOTE: You must make sure that the memory pointed to is large enough.
        void read_bytes(void* p, int32_t size);

        // Reads a field from the buffer.
        ConstFieldPtr read_field();

        // Reads a message from the buffer.
        ConstMessagePtr read_message();

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use Buffer::create() to create an instance.
        Buffer();

        // Reads an item from the buffer using memcpy.
        template <typename T> void readCopyable(T& item);

        // Writes an item to the buffer which can be written with memcpy.
        template <typename T> void writeCopyable(const T& item);

        // Checks that the buffer is large enough to read the specified number of bytes.
        // Throws a MessagingMesh::Exception if the buffer is not large enough.
        void checkBufferSize_Read(size_t bytesRequired);

        // Checks that the buffer has the capacity to hold the number of bytes specified
        // and expands it if it does not.
        // Throws a MessagingMesh::Exception if the buffer required is too large.
        void checkBufferSize_Write(size_t bytesRequired);

        // Expands the buffer by doubling its size.
        void expandBuffer();

        // Updates the position to reflect bytes read from the buffer.
        void updatePosition_Read(int32_t bytesWritten);

        // Updates the position and data-size to reflect bytes written to the buffer.
        void updatePosition_Write(int32_t bytesWritten);

        // Reads the network message size (or as much as can be read) from the buffer.
        size_t readNetworkMessageSize(const char* pBuffer, size_t bufferSize, size_t bufferPosition);

    // Private data...
    private:
        // The initial size we allocate for the buffer...
        const int32_t INITIAL_SIZE = 8192;

        // The buffer...
        char* m_pBuffer = nullptr;
        int32_t m_bufferSize = 0;

        // The current position at which data will be written.
        // This starts after the bytes reserved for the size.
        int32_t m_position = SIZE_SIZE;

        // The size of all data written to the buffer.
        // Note: This includes the size held in the first four bytes.
        int32_t m_dataSize = SIZE_SIZE;

        // True if we have all data for a network message, false if not.
        bool m_hasAllData = false;

        // Buffer when reading the size from a network message.
        // (We may receive the size across multiple network updates.)
        char m_networkMessageSizeBuffer[SIZE_SIZE] = {};
        int m_networkMessageSizeBufferPosition = 0;
    };

} // namespace
