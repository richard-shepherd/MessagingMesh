using System;
using System.Threading;
using MM = MessagingMeshLib.NET;

namespace MMListen
{
    internal class Program
    {
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
                // We parse the command line...
                var commandLine = new CommandLine(args);

                // We show help if requested...
                if (commandLine.hasKey("help"))
                {
                    Console.WriteLine("Usage: MMListen.exe -hostname=[gateway-hostname] -port=[gateway-port] -service=[mm-service-name] -compact=[true/false] -subject=[subject]");
                    return;
                }

                // We connect to the messaging mesh for the service requested...
                var hostname = commandLine.get<string>("hostname");
                var port = commandLine.get<int>("port");
                var service = commandLine.get<string>("service");
                var subject = commandLine.get<string>("subject");
                m_compact = commandLine.get<bool>("compact");
                var connectionParams = new MM.ConnectionParams
                {
                    GatewayHost = hostname,
                    GatewayPort = port,
                    Service = service
                };
                var connection = new MM.Connection(connectionParams);

                // We subscribe to the subject...
                var subscription = connection.subscribe(subject, onMessage);

                // We process incoming messages until Enter is pressed...
                processMessages(connection);
                subscription.Dispose();
                connection.Dispose();
            }
            catch (Exception ex)
            {
                MM.Logger.fatal(ex.Message);
            }
        }

        /// <summary>
        /// Called when we receive an update to the subject we are subscribed to.
        /// </summary>
        private static void onMessage(MM.Connection connection, string subject, string replySubject, MM.Message message, object tag)
        {
            try
            {
                // We log the message...
                MM.Logger.info(message.toMMListenString(subject, m_compact));
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Exception: {ex.Message}");
            }
        }

        /// <summary>
        /// Called when a message is logged by the messaging-mesh.
        /// </summary>
        private static void onMessagingMeshLogMessage(MM.Logger.LogLevel logLevel, string message)
        {
            Console.WriteLine($"{DateTime.UtcNow:HH:mm:ss.fff} ({logLevel}): {message}");
        }

        /// <summary>
        /// Processes messages until Enter is pressed.
        /// </summary>
        public static void processMessages(MM.Connection connection, int millisecondsTimeout = 100)
        {
            MM.Logger.info("Press Enter to exit");
            for (; ; )
            {
                // We process messages...
                var info = connection.processMessageQueue(millisecondsTimeout);

                // We check for Enter...
                if (Console.KeyAvailable)
                {
                    var key = Console.ReadKey(intercept: true);
                    if (key.Key == ConsoleKey.Enter) break;
                }
            }
        }

        // Whether we want to log compact messages...
        private static bool m_compact = false;
    }
}
