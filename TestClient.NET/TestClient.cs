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
                var commandLine = new MM.CommandLine(args);
                commandLine.addDefault("gateway-hostname", "127.0.0.1");
                commandLine.addDefault("gateway-port", "5050");
                commandLine.addDefault("service", "VULCAN");

                if (commandLine.hasKey("sub"))
                {
                    SmallMessageSubscriber.start(commandLine);
                }
                else if (commandLine.hasKey("pub"))
                {
                    SmallMessagePublisher.start(commandLine);
                }
                else if (commandLine.hasKey("sub-blob"))
                {
                    BLOBSubscriber.start(commandLine);
                }
                else if (commandLine.hasKey("pub-blob"))
                {
                    BLOBPublisher.start(commandLine);
                }
                else if (commandLine.hasKey("client"))
                {
                    Client.start(commandLine);
                }
                else if (commandLine.hasKey("server"))
                {
                    Server.start(commandLine);
                }
                else if (commandLine.hasKey("ping"))
                {
                    Pinger.start(commandLine);
                }
                else if (commandLine.hasKey("pong"))
                {
                    Ponger.start(commandLine);
                }
                else if (commandLine.hasKey("pub-mixed"))
                {
                    MixedLoadPublisher.start(commandLine);
                }
                else if (commandLine.hasKey("sub-mixed"))
                {
                    MixedLoadSubscriber.start(commandLine);
                }
                else
                {
                    MM.Logger.error("Usage: TestClient.NET.exe -sub/-pub, -sub-blob/-pub-blob, -sub-mixed/-pub-mixed, -client/-server, -ping/-pong");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
