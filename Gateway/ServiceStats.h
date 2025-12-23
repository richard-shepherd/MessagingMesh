#include <cstdint>
#include <chrono>
#include <string>
#include <unordered_map>

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
        // Stats, either total or per subject used by the Snapshot (below).
        struct Stats
        {
            std::string Subject;
            uint64_t MessagesProcessed = 0;
            uint64_t BytesProcessed = 0;
            double MessagesPerSecond = 0.0;
            double MegaBitsPerSecond = 0.0;
        };
        using VecStats = std::vector<Stats>;

        // Snapshot calculated every N seconds.
        struct StatsSnapshot 
        {
            std::string ServiceName;
            double DurationSeconds = 0.0;
            Stats Total;
            VecStats TopSubjects_MessagesPerSecond;
            VecStats TopSubjects_MegaBitsPerSecond;
        };

    // Public methods...
    public:
        // Constructor.
        ServiceStats(const std::string& serviceName);

        // Resets the stats.
        void reset();

        // Adds a message to the stats.
        void add(const std::string& subject, size_t messageSizeBytes);

        // Gets a stats snapshot (and resets the stats).
        StatsSnapshot getSnapshot();

        // Logs stats.
        void log();

    // Private types...
    private:
        // Stats used internally as stats as collected.
        struct InternalStats
        {
            uint64_t MessagesProcessed = 0;
            uint64_t BytesProcessed = 0;
        };

    // Private functions...
    private:
        // Gets the top N items from stats-per-subject sorted by the comparator provided.
        template<typename Comparator>
        VecStats getTopItems(double elapsedSeconds, size_t n, Comparator comparator) const;

        // Converts internal stats to stats for logging / publication.
        Stats toStats(const InternalStats& internalStats, double elapsedSeconds, const std::string& subject) const;

    // Private data...
    private:
        // The name of the service for which we are collecting stats...
        std::string m_serviceName;
            
        // The time we started recording stats...
        std::chrono::steady_clock::time_point m_startTime;

        // Totals for the current time period...
        InternalStats m_total;

        // Stats per subject for the current time period...
        std::unordered_map<std::string, InternalStats> m_statsPerSubject;
    };

    // Serialize Stats struct to JSON.
    template<typename JSONType>
    inline void to_json(JSONType& j, const ServiceStats::Stats& stats)
    {
        j = JSONType{
            {"Subject", stats.Subject},
            {"MessagesProcessed", stats.MessagesProcessed},
            {"BytesProcessed", stats.BytesProcessed},
            {"MessagesPerSecond", stats.MessagesPerSecond},
            {"MegaBitsPerSecond", stats.MegaBitsPerSecond}
        };
    }

    // Serialize StatsSnapshot struct to JSON.
    template<typename JSONType>
    inline void to_json(JSONType& j, const ServiceStats::StatsSnapshot& snapshot)
    {
        j = JSONType{
            {"ServiceName", snapshot.ServiceName},
            {"DurationSeconds", snapshot.DurationSeconds},
            {"Total", snapshot.Total},
            {"TopSubjects_MessagesPerSecond", snapshot.TopSubjects_MessagesPerSecond},
            {"TopSubjects_MegaBitsPerSecond", snapshot.TopSubjects_MegaBitsPerSecond}
        };
    }

} // namespace


