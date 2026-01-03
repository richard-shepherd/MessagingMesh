#include "ServiceStats.h"
#include <algorithm>
#include <Logger.h>
#include <nlohmann/json.hpp>
using namespace MessagingMesh;

// Constructor.
ServiceStats::ServiceStats(const std::string& serviceName, const std::string& gatewayName) :
    m_serviceName(serviceName),
    m_gatewayName(gatewayName)
{
    reset();
}

// Resets the stats.
void ServiceStats::reset()
{
    m_startTime = std::chrono::steady_clock::now();
    m_total = InternalStats();
    m_statsPerSubject.clear();
}

// Adds a message to the stats.
void ServiceStats::add(const std::string& subject, size_t messageSizeBytes)
{
    // We update the totals...
    ++m_total.MessagesProcessed;
    m_total.BytesProcessed += messageSizeBytes;

    // We update the stats per subject...
    auto& subjectStats = m_statsPerSubject[subject];
    ++subjectStats.MessagesProcessed;
    subjectStats.BytesProcessed += messageSizeBytes;
}

// Gets a stats snapshot (and resets the stats).
ServiceStats::StatsSnapshot ServiceStats::getSnapshot()
{
    // We find the time since we started recording the stats...
    auto endTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime);
    auto elapsedSeconds = elapsed.count() / 1000000.0;

    // We calculate the snapshot...
    StatsSnapshot snapshot;
    snapshot.ServiceName = m_serviceName;
    snapshot.GatewayName = m_gatewayName;
    snapshot.DurationSeconds = elapsedSeconds;

    // Total...
    snapshot.Total = toStats(m_total, elapsedSeconds, "");

    // Top items by messages/second...
    snapshot.TopSubjects_MessagesPerSecond = getTopItems(
        elapsedSeconds,
        15,
        [](const auto& a, const auto& b) {return a.second.MessagesProcessed > b.second.MessagesProcessed;});

    // Top items by Megabits/second...
    snapshot.TopSubjects_MegaBitsPerSecond = getTopItems(
        elapsedSeconds,
        15,
        [](const auto& a, const auto& b) {return a.second.BytesProcessed > b.second.BytesProcessed;});

    // We reset the counters and return the stats...
    reset();
    return snapshot;
}

// Gets a stats snapshot as JSON (and resets the stats).
std::string ServiceStats::getSnapshotAsJSON(bool indented)
{
    // We get the stats snapshot...
    auto snapshot = getSnapshot();

    // We convert it to JSON and log it...
    nlohmann::ordered_json json = snapshot;
    return indented ? json.dump(4) : json.dump();
}

// Gets the top N items from stats-per-subject sorted by the comparator provided.
template<typename Comparator>
std::vector<ServiceStats::Stats> ServiceStats::getTopItems(double elapsedSeconds, size_t n, Comparator comparator) const
{
    // We convert the stats per subject to a vector...
    std::vector<std::pair<std::string, InternalStats>> vecStatsPerSubject(m_statsPerSubject.begin(), m_statsPerSubject.end());

    // We find the number of items (in case 'n' is larger than the number of items we have)...
    auto numItems = std::min(n, vecStatsPerSubject.size());

    // We find the top items...
    std::partial_sort(
        vecStatsPerSubject.begin(),
        vecStatsPerSubject.begin() + numItems,
        vecStatsPerSubject.end(),
        comparator);
    vecStatsPerSubject.resize(numItems);

    // We convert the internal stats to stats for logging / publication...
    VecStats topStats;
    topStats.reserve(numItems);
    for (const auto& [subject, internalStats] : vecStatsPerSubject)
    {
        topStats.push_back(toStats(internalStats, elapsedSeconds, subject));
    }
    return topStats;
}

// Converts internal stats to stats for logging / publication.
ServiceStats::Stats ServiceStats::toStats(const InternalStats& internalStats, double elapsedSeconds, const std::string& subject) const
{
    auto messagesPerSecond = elapsedSeconds > 0.0 ? internalStats.MessagesProcessed / elapsedSeconds : 0.0;
    auto bytesPerSeconds = elapsedSeconds > 0.0 ? internalStats.BytesProcessed / elapsedSeconds : 0.0;
    auto megaBitsPerSecond = bytesPerSeconds * 8 / (1024 * 1024);
    return Stats
    {
        subject,
        internalStats.MessagesProcessed,
        internalStats.BytesProcessed,
        messagesPerSecond,
        megaBitsPerSecond
    };
}

// Logs stats.
void ServiceStats::log()
{
    Logger::info(std::format("STATS: {}", getSnapshotAsJSON(true)));
}


