using System;
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
        /// Main.
        /// </summary>
        static void Main(string[] args)
        {
            // We name the thread...
            Thread.CurrentThread.Name = "MAIN";

            // We connect to the logger...
            MM.Logger.registerCallback(onMessagingMeshLogMessage);
            MM.Logger.info($"MM client version={MM.Connection.Version}");

            try
            {
                if (args.Length > 0 && args[0] == "-sub")
                {
                    SmallMessageSubscriber.start();
                }
                else if (args.Length > 0 && args[0] == "-pub")
                {
                    SmallMessagePublisher.start();
                }
                else if (args.Length > 0 && args[0] == "-sub-blob")
                {
                    BLOBSubscriber.start();
                }
                else if (args.Length > 0 && args[0] == "-pub-blob")
                {
                    BLOBPublisher.start();
                }
                else if (args.Length > 0 && args[0] == "-client")
                {
                    Client.start();
                }
                else if (args.Length > 0 && args[0] == "-server")
                {
                    Server.start();
                }
                else if (args.Length > 0 && args[0] == "-ping")
                {
                    Pinger.start();
                }
                else if (args.Length > 0 && args[0] == "-pong")
                {
                    Ponger.start();
                }
                else if (args.Length > 0 && args[0] == "-pub-mixed")
                {
                    MixedLoadPublisher.start();
                }
                else if (args.Length > 0 && args[0] == "-sub-mixed")
                {
                    MixedLoadSubscriber.start();
                }
                else
                {
                    MM.Logger.error("Usage: TestClient.NET.exe -sub/-pub");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
