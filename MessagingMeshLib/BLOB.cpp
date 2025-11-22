#include "BLOB.h"
#include "Exception.h"
using namespace MessagingMesh;

// Constructor: From data.
// NOTE: The constructor is private. Use one of the static create() methods to create an instance.
BLOB::BLOB(void* pData, size_t length, Ownership ownership) :
    m_length(length),
    m_ownership(ownership)
{
    switch (ownership)
    {
    case Ownership::TAKE_OWNERSHIP:
    case Ownership::HOLD_REFERENCE:
        m_pData = pData;
        break;

    case Ownership::TAKE_COPY:
        m_pData = new char[length];
        std::memcpy(m_pData, pData, length);
        break;
    }
}

// Constructor: From a Buffer.
// NOTE: The constructor is private. Use one of the static create() methods to create an instance.
BLOB::BLOB(BufferPtr pBuffer, void* pData, size_t length, Ownership ownership) :
    m_length(length),
    m_ownership(ownership)
{
    switch (ownership)
    {
    case Ownership::HOLD_REFERENCE:
        m_pData = pData;
        m_pBuffer = pBuffer;
        break;

    case Ownership::TAKE_COPY:
        m_pData = new char[length];
        std::memcpy(m_pData, pData, length);
        break;

    case Ownership::TAKE_OWNERSHIP:
        throw Exception("TAKE_OWNERSHIP is not a valid option when creating a BLOB from a Buffer.");
    }
}


// Destructor.
BLOB::~BLOB()
{
    if (m_ownership == Ownership::TAKE_OWNERSHIP || m_ownership == Ownership::TAKE_COPY)
    {
        delete[] m_pData;
    }
}
