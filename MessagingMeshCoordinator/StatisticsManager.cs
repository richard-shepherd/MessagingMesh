using MessagingMeshLib.NET;
using System;
using MM = MessagingMeshLib.NET;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Subscribes to statistics published by gateways.
    /// - Aggregates statistics from multiple gateways managing the same service.
    /// - Stores statistics in the database.
    /// - Makes statistics available for the web UI.
    /// </summary>
    internal class StatisticsManager : IDisposable
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public StatisticsManager(MM.Connection connection)
        {
            m_connection = connection;

            // We subscribe the gateway stats...
            m_statsSubscription = m_connection.subscribe("GATEWAY.STATS.>", onStatsMessage);
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            // We stop the stats subscription...
            m_statsSubscription.Dispose();

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region Private functions

        /// <summary>
        /// Called when we receive a stats update from a gateway.
        /// </summary>
        private void onStatsMessage(Connection connection, string subject, string replySubject, Message message, object tag)
        {
            try
            {
                MM.Logger.info($"Got stats for: {subject}");
            }
            catch (Exception ex)
            {
                MM.Logger.log(ex);
            }
        }

        #endregion

        #region Private data

        // The messaging-mesh connection...
        private readonly MM.Connection m_connection;

        // The stats subscription...
        private readonly IDisposable m_statsSubscription;

        #endregion
    }
}
