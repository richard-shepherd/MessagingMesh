#include "ServiceStats.h"
#include <Logger.h>
using namespace MessagingMesh;

// Constructor.
ServiceStats::ServiceStats()
{
    reset();
}

// Resets the stats.
void ServiceStats::reset()
{
    m_startTime = std::chrono::steady_clock::now();
    m_messagesProcessed = 0;
    m_bytesProcessed = 0;
}

// Adds a message to the stats.
void ServiceStats::recordMessage(size_t messageSizeBytes)
{
    ++m_messagesProcessed;
    m_bytesProcessed += messageSizeBytes;
}

// Gets a stats snapshot (and resets the stats).
ServiceStats::StatsSnapshot ServiceStats::getSnapshot()
{
    // We find the time since we started recording the stats...
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime);
    auto elapsedSeconds = elapsed.count() / 1000000.0;

    // We calculate the snapshot...
    auto messagesPerSecond = elapsedSeconds > 0.0 ? m_messagesProcessed / elapsedSeconds : 0.0;
    auto bytesPerSeconds = elapsedSeconds > 0.0 ? m_bytesProcessed / elapsedSeconds : 0.0;
    auto megaBitsPerSecond = bytesPerSeconds * 8 / (1024 * 1024);
    auto snapshot = StatsSnapshot
    {
        m_messagesProcessed,
        m_bytesProcessed,
        elapsedSeconds,
        messagesPerSecond,
        megaBitsPerSecond
    };

    // We reset the counters and return the stats...
    reset();
    return snapshot;
}

// Logs stats.
void ServiceStats::log()
{
    // We get the stats snapshot and log it...
    auto snapshot = getSnapshot();
    Logger::info(std::format("Stats: Msg/sec={:.2f}, Mb/s={:.2f}", snapshot.MessagesPerSecond, snapshot.MegaBitsPerSecond));
}


