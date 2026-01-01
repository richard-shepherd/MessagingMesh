using Microsoft.Extensions.Hosting;
using System.Threading;
using System.Threading.Tasks;
using MM = MessagingMeshLib.NET;

namespace MessagingMeshCoordinator.Services
{
    /// <summary>
    /// Allows the Coordinator to be hosted as a service by ASP.NET.
    /// </summary>
    public class CoordinatorHostedService : IHostedService
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public CoordinatorHostedService(Coordinator coordinator)
        {
            m_coordinator = coordinator;
        }

        #endregion

        #region IHostedService implementation

        /// <summary>
        /// Starts the service.
        /// </summary>
        public Task StartAsync(CancellationToken cancellationToken)
        {
            MM.Logger.info("Starting CoordinatorHostedService");

            // We run the coordinator's messaging-mesh loop on its own thread...
            m_cancellationTokenSource = new();
            m_messageLoopThread = new Thread(() =>
            {
                m_coordinator.processMessages(m_cancellationTokenSource.Token);
            });
            m_messageLoopThread.Name = "COORDINATOR";
            m_messageLoopThread.IsBackground = true;
            m_messageLoopThread.Start();

            return Task.CompletedTask;
        }

        /// <summary>
        /// Stops the service.
        /// </summary>
        public Task StopAsync(CancellationToken cancellationToken)
        {
            MM.Logger.info("Stopping CoordinatorHostedService");

            // We signal the coordinator loop to stop...
            m_cancellationTokenSource?.Cancel();
            m_messageLoopThread?.Join(millisecondsTimeout: 2000);

            return Task.CompletedTask;
        }

        #endregion

        #region Private data

        // The coordinator (managing messaging-mesh interactions)...
        private readonly Coordinator m_coordinator;

        // A thread to run the message loop and a cancellation token to stop it...
        private Thread m_messageLoopThread;
        private CancellationTokenSource m_cancellationTokenSource;

        #endregion
    }
}
