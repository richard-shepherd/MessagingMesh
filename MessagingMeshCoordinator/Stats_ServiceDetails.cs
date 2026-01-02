using System.Collections.Generic;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Detailed stats for one service, aggregated from all gateways managing the service.
    /// </summary>
    public class Stats_ServiceDetails
    {
        /// <summary>
        /// Gets or sets the service name.
        /// </summary>
        public string ServiceName { get; set; } = "";

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
