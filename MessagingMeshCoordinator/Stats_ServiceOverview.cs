namespace MessagingMeshCoordinator
{
    /// <summary>
    /// An overview of aggregated stats for a messaging-mesh Service.
    /// </summary>
    public class Stats_ServiceOverview
    {
        /// <summary>
        /// Gets or sets the service name.
        /// </summary>
        public string ServiceName { get; set; } = "";

        /// <summary>
        /// Gets or sets the messages per seconds in the reporting interval for the subject.
        /// </summary>
        public double MessagesPerSecond { get; set; } = 0.0;

        /// <summary>
        /// Gets or sets the Mb/sec in the reporting interval for the subject.
        /// </summary>
        public double MegaBitsPerSecond { get; set; } = 0.0;
    }
}
