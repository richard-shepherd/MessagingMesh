using System;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class Utils
    {
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
                MM.Logger.info($"P={info.MessagesProcessed}, Q={info.QueueSize}");

                // We check for Enter...
                if (Console.KeyAvailable)
                {
                    var key = Console.ReadKey(intercept: true);
                    if (key.Key == ConsoleKey.Enter) break;
                }
            }
        }

    }
}
