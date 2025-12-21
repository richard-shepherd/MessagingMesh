#include <cstdint>
#include <chrono>

#pragma once

namespace MessagingMesh
{
    /// <summary>
    /// Statistics reported by ServiceManager for a service.
    /// </summary>
    class ServiceStats
    {
    // Public types...
    public:
        struct StatsSnapshot 
        {
            uint64_t MessagesProcessed;
            uint64_t BytesProcessed;
            double DurationSeconds;
            double MessagesPerSecond;
            double MegaBitsPerSecond;
        };

    // Public methods...
    public:
        // Constructor.
        ServiceStats();

        // Resets the stats.
        void reset();

        // Adds a message to the stats.
        void recordMessage(size_t messageSizeBytes);

        // Gets a stats snapshot (and resets the stats).
        StatsSnapshot getSnapshot();

        // Logs stats.
        void log();

    // Private data...
    private:
        // The time we started recording stats...
        std::chrono::steady_clock::time_point m_startTime;

        // The number of messages sent since the start time...
        uint64_t m_messagesProcessed;

        // The number of bytes sent since the start time...
        uint64_t m_bytesProcessed;
    };
} // namespace


