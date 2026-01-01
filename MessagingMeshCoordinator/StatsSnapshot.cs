using System;
using System.Collections.Generic;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Parsed version of the JSON stats sent by gateways.
    /// This is equivalent to the ServiceStats::StatsSnapshot from the Gateway.
    /// </summary>
    internal class StatsSnapshot
    {
        /// <summary>
        /// Stats, either total or per subject.
        /// </summary>
        public class Stats
        {
            /// <summary>
            /// Gets or sets the subject. 
            /// This is "" if the stats are total stats across all subjects.
            /// </summary>
            public string Subject { get; set; } = "";

            /// <summary>
            /// Gets or sets the number of messages processed in the reporting interval for the subject.
            /// </summary>
            public ulong MessagesProcessed { get; set; } = 0;

            /// <summary>
            /// Gets or sets the bytes processed in the reporting interval for the subject.
            /// </summary>
            public long BytesProcessed { get; set; } = 0;

            /// <summary>
            /// Gets or sets the messages per seconds in the reporting interval for the subject.
            /// </summary>
            public double MessagesPerSecond { get; set; } = 0.0;

            /// <summary>
            /// Gets or sets the Mb/sec in the reporting interval for the subject.
            /// </summary>
            public double MegaBitsPerSecond { get; set; } = 0.0;
        }

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
        public Stats Total { get; set; } = new();

        /// <summary>
        /// Gets or sets stats for the top subjects by msg/sec processed in the reporting interval.
        /// </summary>
        public List<Stats> TopSubjects_MessagesPerSecond { get; set; } = new();

        /// <summary>
        /// Gets or sets stats for the top subjects by Mb/sec processed in the reporting interval.
        /// </summary>
        public List<Stats> TopSubjects_MegaBitsPerSecond { get; set; } = new();
    }
}
