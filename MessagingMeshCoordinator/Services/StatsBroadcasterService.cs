using MessagingMeshCoordinator.Hubs;
using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.Hosting;
using System;
using System.Threading;
using System.Threading.Tasks;
using MM = MessagingMeshLib.NET;

namespace MessagingMeshCoordinator.Services
{
    /// <summary>
    /// Broadcasts stats to connected clients on a timer.
    /// </summary>
    public class StatsBroadcasterService : IHostedService
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public StatsBroadcasterService(IHubContext<StatsHub> hubContext, StatisticsManager statisticsManager)
        {
            m_hubContext = hubContext;
            m_statisticsManager = statisticsManager;
        }

        #endregion

        #region IHostedService implementation

        /// <summary>
        /// Starts the service.
        /// </summary>
        public Task StartAsync(CancellationToken cancellationToken)
        {
            MM.Logger.info("Starting StatsBroadcasterService");

            // We create the timer to broadcast stats every second...
            m_broadcastTimer = new Timer(broadcastServiceOverviews, null, TimeSpan.FromSeconds(1), TimeSpan.FromSeconds(1));

            return Task.CompletedTask;
        }

        /// <summary>
        /// Stops the service.
        /// </summary>
        public Task StopAsync(CancellationToken cancellationToken)
        {
            MM.Logger.info("Stopping StatsBroadcasterService");

            // We clean up the timer...
            m_broadcastTimer?.Dispose();

            return Task.CompletedTask;
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Called when the timer ticks to boradcast the latests service overviews.
        /// </summary>
        private void broadcastServiceOverviews(object state)
        {
            try
            {
                // We get the current stats and broadcast them.
                // Note: This does not use await partly as we do not need to await, and also because the timer 
                //       callback makes this hard anyway.
                var serviceOverviews = m_statisticsManager.getServiceOverviews();
                _ = m_hubContext.Clients.All.SendAsync("ServiceOverviewsUpdate", serviceOverviews);
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Error broadcasting service overviews: {ex.Message}");
            }
        }

        #endregion

        #region Private data

        // The Hub to which this service belongs...
        private readonly IHubContext<StatsHub> m_hubContext;

        // The statistics manager...
        private readonly StatisticsManager m_statisticsManager;

        // Timer for broadcasting stats...
        private Timer m_broadcastTimer;
        
        #endregion
    }
}
