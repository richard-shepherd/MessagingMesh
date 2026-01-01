using System;
using System.Collections.Generic;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Parsed version of the JSON stats sent by gateways.
    /// This is equivalent to the ServiceStats::StatsSnapshot from the Gateway.
    /// </summary>
    internal class Stats_Snapshot
    {
        /// <summary>
        /// Gets or sets the service name.
        /// </summary>
        public string ServiceName { get; set; } = "";

        /// <summary>
        /// Gets or sets the gateway name.
        /// </summary>
        public string GatewayName { get; set; } = "";

        /// <summary>
        /// Gets the duration in seconds of the reporting interval.
        /// </summary>
        public double DurationSeconds { get; set; } = 0.0;

        /// <summary>
        /// Gets or sets the total stats across all subjects.
        /// </summary>
        public Stats_PerSubject Total { get; set; } = new();

        /// <summary>
        /// Gets or sets stats for the top subjects by msg/sec processed in the reporting interval.
        /// </summary>
        public List<Stats_PerSubject> TopSubjects_MessagesPerSecond { get; set; } = new();

        /// <summary>
        /// Gets or sets stats for the top subjects by Mb/sec processed in the reporting interval.
        /// </summary>
        public List<Stats_PerSubject> TopSubjects_MegaBitsPerSecond { get; set; } = new();
    }
}
