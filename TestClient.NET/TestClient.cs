using MessagingMeshLib.NET;
using System;
using System.Runtime.Remoting.Messaging;
using System.Security.Policy;
using System.Threading;
using MM = MessagingMeshLib.NET;

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
        /// Called when we receive a message.
        /// </summary>
        private static void onMessage(Connection connection, string subject, string replySubject, Message message, object tag)
        {
            try
            {
                var value = message.getSignedInt("#");
                if (value % 1000000 == 0)
                {
                    MM.Logger.info($"Update to {subject}: {value}");
                }
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Exception: {ex.Message}");
            }
        }

        /// <summary>
        /// Subscribes to messages.
        /// </summary>
        private static void subscribe()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We make subscriptions...
            var s1 = connection.subscribe("A.X", onMessage);
            var s2 = connection.subscribe("A.A", onMessage);
            var s3 = connection.subscribe("A.B", onMessage);
            var s4 = connection.subscribe("C.D", onMessage);

            MM.Logger.info("Press Enter to exit");
            Console.ReadLine();
            connection.Dispose();
        }

        /// <summary>
        /// Publishes messages.
        /// </summary>
        private static void publish()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We send updates...
            MM.Logger.info("Sending data");
            for (int i = 1; i <= 10000000; ++i)
            {
                {
                    var message = new MM.Message();
                    message.addSignedInt("#", i);
                    connection.sendMessage("A.B", message);
                }
            }

            MM.Logger.info("Press Enter to exit");
            Console.ReadLine();
            connection.Dispose();
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

            if (args.Length > 0 && args[0] == "-sub")
            {
                subscribe();
            }
            else if(args.Length > 0 && args[0] == "-pub")
            {
                publish();
            }
            else
            {
                MM.Logger.error("Usage: TestClient.NET.exe -sub/-pub");
            }
        }
    }
}
