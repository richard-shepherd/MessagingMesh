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

            // We send updates...
            MM.Logger.info("Sending data");
            for (int i = 1; i <= 50000000; ++i)
            {
                {
                    var message = new MM.Message();
                    message.addSignedInt("#", i);
                    connection.sendMessage("A.B", message);
                    //if(i%100000 == 0)
                    //{
                    //    Thread.Sleep(100);
                    //}
                }
            }

            MM.Logger.info("Press Enter to exit");
            Console.ReadLine();
            connection.Dispose();
        }
    }
}
