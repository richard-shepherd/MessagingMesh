using MessagingMeshLib.NET;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
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
                // The stats subject looks like: GATEWAY.STATS.[gateway-name].[service-name]
                if(!subject.StartsWith(STATS_MESSAGE_PREFIX))
                {
                    throw new Exception($"Unexpected subject: {subject}. Expected to start with '{STATS_MESSAGE_PREFIX}'.");
                }

                // The stats are sent as JSON in the SERVICE_STATS field. We parse this...
                var statsSnapshotJSON = message.getString("SERVICE_STATS");
                var statsSnapshot = JsonConvert.DeserializeObject<Stats_Snapshot>(statsSnapshotJSON);

                // The stats subject looks like: GATEWAY.STATS.[gateway-name].[service-name]
                // We store the stats keyed by gateway and service.
                var key = subject.Substring(STATS_MESSAGE_PREFIX.Length);
                m_statsSnapshots[key] = statsSnapshot;

                MM.Logger.info($"Got stats for: {subject}. Total msg/sec={statsSnapshot.Total.MessagesPerSecond:0.0}");
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

        // Stats snapshots keyed by (gateway-name, service-name)...
        private readonly Dictionary<string, Stats_Snapshot> m_statsSnapshots = new();

        // The prefix for stats messages...
        private const string STATS_MESSAGE_PREFIX = "GATEWAY.STATS.";

        #endregion
    }
}
