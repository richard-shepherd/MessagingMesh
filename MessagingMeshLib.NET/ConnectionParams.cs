namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Parameters requesting a connection to the messaging mesh.
    /// Passed to the constructor of the Connection class.
    /// </summary>
    public class ConnectionParams
    {
        #region Properties

        /// <summary>
        /// Gets or sets the gateway hostname or IP address.
        /// </summary>
        public string GatewayHost { get; set; } = "";

        /// <summary>
        /// Gets or sets the gateway port.
        /// </summary>
        public int GatewayPort { get; set; } = 0;

        /// <summary>
        /// Gets or sets the messaging-mesh service the the connection joins.
        /// </summary>
        public string Service { get; set; } = "";

        #endregion
    }
}
