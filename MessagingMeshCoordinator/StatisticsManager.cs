using MessagingMeshLib.NET;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using MM = MessagingMeshLib.NET;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Subscribes to statistics published by gateways.
    /// - Aggregates statistics from multiple gateways managing the same service.
    /// - Stores statistics in the database.
    /// - Makes statistics available for the web UI.
    /// </summary>
    public class StatisticsManager : IDisposable
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

        /// <summary>
        /// Returns a summary of stats per service.
        /// </summary>
        public List<Stats_ServiceOverview> getServiceOverviews()
        {
            lock(m_statsSnapshotsLocker)
            {
                var serviceOverviews = new List<Stats_ServiceOverview>();

                // We find the list of services and find the total stats for each of them...
                var serviceNames = m_statsSnapshots.Values.Select(x => x.ServiceName).Distinct().OrderBy(x => x);
                foreach (var serviceName in serviceNames)
                {
                    serviceOverviews.Add(getServiceOverview(serviceName));
                }
                return serviceOverviews;
            }
        }

        /// <summary>
        /// Gets aggregated service details for the service name specified.
        /// </summary>
        public Stats_ServiceDetails getServiceDetails(string serviceName)
        {
            lock (m_statsSnapshotsLocker)
            {
                var serviceDetails = new Stats_ServiceDetails();
                serviceDetails.ServiceName = serviceName;

                // We fill in the totals...
                var serviceOverview = getServiceOverview(serviceName);
                serviceDetails.TotalMessagesPerSecond = serviceOverview.MessagesPerSecond;
                serviceDetails.TotalMegaBitsPerSecond = serviceOverview.MegaBitsPerSecond;

                // We find the snapshots for the service...
                var snapshots = m_statsSnapshots.Values.Where(x => x.ServiceName == serviceName);

                // We aggregate the stats...
                var dict_TopSubjects_MessagesPerSecond = new Dictionary<string, Stats_PerSubject>();
                var dict_TopSubjects_MegabitsPerSecond = new Dictionary<string, Stats_PerSubject>();
                foreach(var snapshot in snapshots)
                {
                    mergeTopSubjects(snapshot.TopSubjects_MessagesPerSecond, dict_TopSubjects_MessagesPerSecond);
                    mergeTopSubjects(snapshot.TopSubjects_MegaBitsPerSecond, dict_TopSubjects_MegabitsPerSecond);
                }

                // We have a list of the top subjects (from all gateways). We convert these to lists
                // and take the top ten items...
                serviceDetails.TopSubjects_MessagesPerSecond = dict_TopSubjects_MessagesPerSecond.Values
                    .OrderBy(x => x.MessagesPerSecond)
                    .Take(10)
                    .ToList();
                serviceDetails.TopSubjects_MegaBitsPerSecond = dict_TopSubjects_MegabitsPerSecond.Values
                    .OrderBy(x => x.MegaBitsPerSecond)
                    .Take(10)
                    .ToList();

                return serviceDetails;
            }
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
        /// Returns the service overview for the service name specified.
        /// </summary>
        private Stats_ServiceOverview getServiceOverview(string serviceName)
        {
            var snapshots = m_statsSnapshots.Values.Where(x => x.ServiceName == serviceName);
            return new Stats_ServiceOverview
            {
                ServiceName = serviceName,
                MessagesPerSecond = snapshots.Sum(x => x.Total.MessagesPerSecond),
                MegaBitsPerSecond = snapshots.Sum(x => x.Total.MegaBitsPerSecond)
            };
        }

        /// <summary>
        /// Merges the list of top-subjects into the dictionary.
        /// </summary>
        private void mergeTopSubjects(List<Stats_PerSubject> list, Dictionary<string, Stats_PerSubject> dict)
        {
            foreach(var listItem in list)
            {
                if(!dict.TryGetValue(listItem.Subject, out var dictItem))
                {
                    dictItem = new();
                    dictItem.Subject = listItem.Subject;
                    dict.Add(listItem.Subject, dictItem);
                }
                dictItem.MessagesProcessed += listItem.MessagesProcessed;
                dictItem.BytesProcessed += listItem.BytesProcessed;
                dictItem.MessagesPerSecond += listItem.MessagesPerSecond;
                dictItem.MegaBitsPerSecond += listItem.MegaBitsPerSecond;
            }
        }

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
                lock(m_statsSnapshotsLocker)
                {
                    m_statsSnapshots[key] = statsSnapshot;
                }
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

        // Stats snapshots keyed by (gateway-name, service-name), and a locker for it...
        private readonly Dictionary<string, Stats_Snapshot> m_statsSnapshots = new();
        private readonly object m_statsSnapshotsLocker = new();

        // The prefix for stats messages...
        private const string STATS_MESSAGE_PREFIX = "GATEWAY.STATS.";

        #endregion
    }
}
