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
    internal class Coordinator : IDisposable
    {
        #region Main

        /// <summary>
        /// Main.
        /// </summary>
        static void Main(string[] args)
        {
            // We name the thread...
            Thread.CurrentThread.Name = "MAIN";

            // We connect to the logger...
            MM.Logger.registerCallback(onMessagingMeshLogMessage);
            MM.Logger.info($"MM client version={MM.Connection.Version}");

            try
            {
                // We create the coordinator and run its message loop...
                var coordinator = new Coordinator();
                coordinator.processMessages();

                // The message loop has been terminated so we clean up...
                coordinator.Dispose();
            }
            catch (Exception ex)
            {
                MM.Logger.log(ex);
            }
        }

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
        public void processMessages(int millisecondsTimeout = 100)
        {
            MM.Logger.info("Press Enter to exit");
            for (; ; )
            {
                // We process messages...
                var info = m_connection.processMessageQueue(millisecondsTimeout);

                // We check for Enter...
                if (Console.KeyAvailable)
                {
                    var key = Console.ReadKey(intercept: true);
                    if (key.Key == ConsoleKey.Enter) break;
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

        #region Private functions

        /// <summary>
        /// Called when a message is logged by the messaging-mesh.
        /// </summary>
        private static void onMessagingMeshLogMessage(MM.Logger.LogLevel logLevel, string message)
        {
            Console.WriteLine($"{DateTime.UtcNow:HH:mm:ss.fff} ({logLevel}): {message}");
        }

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
