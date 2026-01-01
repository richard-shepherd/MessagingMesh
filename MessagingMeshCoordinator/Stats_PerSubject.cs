namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Stats for a subject.
    /// The Subject itself will be an empty string if the stats are aggregated total stats
    /// across a number of subjects.
    /// </summary>
    public class Stats_PerSubject
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
}
