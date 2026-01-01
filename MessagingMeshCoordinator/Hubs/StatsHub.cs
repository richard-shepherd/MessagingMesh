using Microsoft.AspNetCore.SignalR;
using System;
using System.Threading.Tasks;

namespace MessagingMeshCoordinator.Hubs
{
    /// <summary>
    /// SignalR hub to provide stats to the web UI.
    /// </summary>
    public class StatsHub : Hub
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public StatsHub(StatisticsManager statisticsManager)
        {
            m_statisticsManager = statisticsManager;
        }

        #endregion

        #region Hub implementation

        /// <summary>
        /// Called when a client connects.
        /// </summary>
        public override async Task OnConnectedAsync()
        {
            await base.OnConnectedAsync();

            // We send an inital snapshot of service overviews to the newly connected client...
            var serviceOverviews = m_statisticsManager.getServiceOverviews();
            await Clients.Caller.SendAsync("ServiceOverviewsUpdate", serviceOverviews);
        }

        /// <summary>
        /// Called when a client disconnects.
        /// </summary>
        public override async Task OnDisconnectedAsync(Exception exception)
        {
            await base.OnDisconnectedAsync(exception);
        }

        #endregion

        #region Private data

        // The statistics manager...
        private readonly StatisticsManager m_statisticsManager;

        #endregion
    }
}
