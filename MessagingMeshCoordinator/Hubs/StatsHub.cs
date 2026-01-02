using MessagingMeshCoordinator.Services;
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
        public StatsHub(StatisticsManager statisticsManager, StatsBroadcasterService broadcasterService)
        {
            m_statisticsManager = statisticsManager;
            m_broadcasterService = broadcasterService;
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

        #region Subscriptions

        /// <summary>
        /// Subscribes to details for the specific service.
        /// </summary>
        public async Task SubscribeToServiceDetails(string serviceName)
        {
            await Groups.AddToGroupAsync(Context.ConnectionId, $"ServiceDetails_{serviceName}");
            m_broadcasterService.registerForServiceDetails(serviceName);

            // We send an initial snapshot immediately so the client doesn't wait for the next broadcast...
            var serviceDetails = m_statisticsManager.getServiceDetails(serviceName);
            await Clients.Caller.SendAsync("ServiceDetailsUpdate", serviceDetails);
        }

        /// <summary>
        /// Unsubscribes from details for the specific service.
        /// </summary>
        public async Task UnsubscribeFromServiceDetails(string serviceName)
        {
            await Groups.RemoveFromGroupAsync(Context.ConnectionId, $"ServiceDetails_{serviceName}");
            m_broadcasterService.unregisterFromServiceDetails(serviceName);
        }

        #endregion

        #region Private data

        // The statistics manager...
        private readonly StatisticsManager m_statisticsManager;

        // The stats broadcaster service...
        private readonly StatsBroadcasterService m_broadcasterService;
        #endregion
    }
}
