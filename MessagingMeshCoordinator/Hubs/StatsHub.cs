using Microsoft.AspNetCore.SignalR;
using System;
using System.Threading.Tasks;

namespace MessagingMeshCoordinator.Hubs
{
    public class StatsHub : Hub
    {
        #region Hub implementation

        /// <summary>
        /// Called when a client connects.
        /// </summary>
        public override async Task OnConnectedAsync()
        {
            await base.OnConnectedAsync();
            // Optionally send initial snapshot of current stats
        }

        /// <summary>
        /// Called when a client disconnects.
        /// </summary>
        public override async Task OnDisconnectedAsync(Exception exception)
        {
            await base.OnDisconnectedAsync(exception);
        }

        #endregion
    }
}
