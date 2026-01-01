using System;
using System.Threading;
using MM = MessagingMeshLib.NET;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// MesagingMeshCoordinator
    /// -----------------------
    /// The Coordinator has two main functions:
    /// 
    /// Mesh discovery
    /// --------------
    /// If gateways are configured with DiscoverMeshUsingCoordinator=true, they register services they
    /// are managing with the Coordinator. They also subscribe to the collection of other gateways which
    /// have registered the service so that they can connect the mesh.
    /// 
    /// Statistics and the web UI
    /// -------------------------
    /// Gateways log statistics to the Coordinator. The Coordinator makes these available to a web UI.
    /// Statistics are logged to a database and the web UI allows you to retrieve and view historic 
    /// data.
    /// </summary>
    public class Coordinator : IDisposable
    {
        #region Properties

        /// <summary>
        /// Gets the statistics manager.
        /// </summary>
        public StatisticsManager StatisticsManager => m_statisticsManager;

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Coordinator()
        {
            // We parse the config...
            m_config = ParsedConfig.fromFile("config.json");

            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = m_config.GatewayHostname,
                GatewayPort = m_config.GatewayPort,
                Service = "_COORDINATOR",
                ClientID = "MessagingMeshCoordinator"
            };
            m_connection = new MM.Connection(connectionParams);

            // We create the statistics manager...
            m_statisticsManager = new StatisticsManager(m_connection);
        }

        /// <summary>
        /// Processes messages until Enter is pressed.
        /// </summary>
        public void processMessages(CancellationToken cancellationToken)
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                try
                {
                    // Process messages with a timeout so we can check cancellation
                    m_connection.processMessageQueue(millisecondsTimeout: 100);
                }
                catch (Exception ex)
                {
                    MM.Logger.error($"Error processing messages: {ex.Message}");
                }
            }
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            // We clean up the statistics manager...
            m_statisticsManager.Dispose();

            // We dispose the connection...
            m_connection.Dispose();

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region Private data

        // Parsed config...
        private readonly ParsedConfig m_config;

        // The messaging-mesh connection...
        private readonly MM.Connection m_connection;

        // The statistics manager...
        private readonly StatisticsManager m_statisticsManager;

        #endregion
    }
}
