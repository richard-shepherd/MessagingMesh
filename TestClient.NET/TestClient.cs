using MM = MessagingMeshLib.NET;
using System;
using System.Threading;

namespace TestClient.NET
{
    internal class TestClient
    {

        /// <summary>
        /// Called when a message is logged by the messaging-mesh.
        /// </summary>
        private static void onMessagingMeshLogMessage(MM.Logger.LogLevel logLevel, string message)
        {
            Console.WriteLine($"{DateTime.UtcNow:HH:mm:ss.fff} ({logLevel}): {message}");
        }

        /// <summary>
        /// Main.
        /// </summary>
        static void Main(string[] args)
        {
            // We name the thread...
            Thread.CurrentThread.Name = "MAIN";

            // We connect to the logger...
            MM.Logger.registerCallback(onMessagingMeshLogMessage);

            // We connect to the messaging mesh...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            Console.WriteLine("Press Enter to exit");
            Console.ReadLine();
            connection.Dispose();
        }
    }
}
