#pragma once
#include <memory>
#include <mutex>
#include <libuv/uv.h>
#include "SharedAliases.h"

namespace MessagingMesh
{
    // Holds an OS socket object accessed via a mutex.
    // The mutex is used as these objects are passed between threads.
    class OSSocketHolder
    {
        // Public methods...
    public:
        // Creates an OSSocketHolder instance.
        static OSSocketHolderPtr create() { return OSSocketHolderPtr(new OSSocketHolder()); }

        // Sets the socket.
        void setSocket(uv_os_sock_t socket)
        {
            std::scoped_lock lock(m_mutex);
            m_socket = socket;
        }

        // Gets the socket,
        uv_os_sock_t getSocket() const
        {
            std::scoped_lock lock(m_mutex);
            return m_socket;
        }

    // Private functions...
    private:
        // Constructor.
        // NOTE: The constructor is private. Use OSSocketHolder::create() to create an instance.
        OSSocketHolder() :
            m_socket{}
        {
        }

    // Private data...
    private:
        // OS socket.
        uv_os_sock_t m_socket;

        // Mutex.
        mutable std::mutex m_mutex;
    };
} // namespace

