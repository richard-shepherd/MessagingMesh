#pragma once
#include "SharedAliases.h"

namespace MessagingMesh
{
    /// <summary>
    /// Manages a BLOB, held as:
    /// - void* data
    /// - size_t length
    /// </summary><remarks>
    /// Ownership of the byte array
    /// ---------------------------
    /// There are different options for the ownership of the byte array.
    /// 
    /// - TAKE_OWNERSHIP
    ///   The BLOB will take ownership of the data and delete it in its destructor.
    /// 
    /// - TAKE_COPY:      
    ///   The BLOB will take a copy of the data passed to it. The copy will be deleted in its destructor.
    /// 
    /// - HOLD_REFERENCE: 
    ///   The BLOB holds a reference to data owned externally to it. It will not take ownership of it
    ///   or take a copy of it. When using HOLD_REFERENCE, client code must ensure that the lifetime
    ///   of the data is long enough. For example when sending messages the referenced data must last 
    ///   until Connection.sendMessage() has completed.
    /// 
    /// Ownership when deserialized from a Buffer
    /// -----------------------------------------
    /// When deserialized from a Buffer only TAKE_COPY and HOLD_REFERENCE can be used.
    /// If HOLD_REFERENCE is specified the data held by the BLOB will point to a slice of the data in
    /// the buffer. The BLOB will hold a reference to the Buffer shared-pointer to ensure that the 
    /// lifetime of the buffer matches the lifetime of the BLOB.
    /// </remarks>
    class BLOB
    {
    // Public types...
    public:
        enum class Ownership
        {
            TAKE_OWNERSHIP,
            TAKE_COPY,
            HOLD_REFERENCE
        };

    // Public methods...
    public:
        // Creates a BLOB instance.
        static BLOBPtr create_fromData(void* pData, int32_t length, Ownership ownership) { return BLOBPtr(new BLOB(pData, length, ownership)); }

        // Creates a BLOB instance.
        static BLOBPtr create_fromBuffer(ConstBufferPtr pBuffer, void* pData, int32_t length, Ownership ownership) { return BLOBPtr(new BLOB(pBuffer, pData, length, ownership)); }

        // Destructor.
        ~BLOB();

        // Gets the pointer to the data.
        void* getData() const { return m_pData; }

        // Gets the length.
        int32_t getLength() const { return m_length; }

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use one of the static create() methods to create an instance.
        BLOB(void* pData, int32_t length, Ownership ownership);

        // Constructor.
        // NOTE: The constructor is private. Use one of the static create() methods to create an instance.
        BLOB(ConstBufferPtr pBuffer, void* pData, int32_t length, Ownership ownership);

    // Private data...
    private:
        void* m_pData;
        int32_t m_length;
        Ownership m_ownership;
        ConstBufferPtr m_pBuffer = nullptr;
    };
} // namespace


