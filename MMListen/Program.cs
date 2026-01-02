using System;
using System.Threading;
using MM = MessagingMeshLib.NET;

namespace MMListen
{
    /// <summary>
    /// MMListen
    /// --------
    /// Command line tool which connects to a Messaging Mesh service and
    /// subscribes to a subject. Messages are rendered as text and logged
    /// to the console.
    /// 
    /// Usage - command-line parameters
    /// -------------------------------
    /// -hostname=[gateway-hostname]  (optional, defaults to localhost)
    /// -port=[gateway-port]          (optional, defaults to 5050)
    /// -service=[mm-service-name]    (required)
    /// -compact=[true/false]         (optional, defaults to false; false gives a multi-line rendering of messages; true gives a more compact single line rendering)
    /// -subject="[subject]"          (required; note that the subject should be surrounded by quotes)
    /// </summary>
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
                var commandLine = new MM.CommandLine(args);
                commandLine.addDefault("hostname", "localhost");
                commandLine.addDefault("port", "5050");
                commandLine.addDefault("compact", "false");

                // We show help if requested...
                if (commandLine.hasKey("help"))
                {
                    showUsage();
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
                MM.Logger.info($"Subscribing to '{subject}'");
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

        /// <summary>
        /// Shows the command-line usage.
        /// </summary>
        private static void showUsage()
        {
            Console.WriteLine("MMListen.exe");
            Console.WriteLine("  -hostname=[gateway-hostname]  (optional, default=localhost)");
            Console.WriteLine("  -port=[gateway-port]          (optional, default=5050)");
            Console.WriteLine("  -service=[mm-service-name]    (mandatory)");
            Console.WriteLine("  -subject=\"[subject]\"          (mandatory, subject should be in quotes)");
            Console.WriteLine("  -compact=[true/false]         (optional, default=false)");
        }

        // Whether we want to log compact messages...
        private static bool m_compact = false;
    }
}
