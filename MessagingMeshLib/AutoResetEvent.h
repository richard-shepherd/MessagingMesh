#pragma once
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace MessagingMesh
{
    // C++ equivalent of .NET AutoResetEvent for cross-thread signalling.
    // From: https://stackoverflow.com/questions/8538575/is-there-an-easy-way-to-implement-autoresetevent-in-c0x
    class AutoResetEvent
    {
    // Public methods...
    public:
        // Constructor.
        explicit AutoResetEvent(bool flag = false) :
            m_flag(flag)
        {
        }

        // Deleted methods.
        AutoResetEvent(const AutoResetEvent&) = delete;
        AutoResetEvent& operator=(const AutoResetEvent&) = delete;

        // Sets the signal / event.
        void set()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_flag = true;
            m_signal.notify_all();
        }

        // Resets the signal / event.
        void reset()
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_flag = false;
        }

        // Waits for the signal or for a timeout.
        // Returns true if the signal was seen, false if it timed out.
        bool waitOne(double timeoutSeconds)
        {
            auto timeoutMilliseconds = int(timeoutSeconds * 1000);
            auto endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(timeoutMilliseconds);
            std::unique_lock<std::mutex> lock(m_mutex);
            while (!m_flag)
            {
                auto waitResult = m_signal.wait_until(lock, endTime);
                if (waitResult == std::cv_status::timeout)
                {
                    return false;
                }
            }
            m_flag = false;
            return true;
        }

    // Private data...
    private:
        bool m_flag;
        std::mutex m_mutex;
        std::condition_variable m_signal;
    };
}  // namespace


