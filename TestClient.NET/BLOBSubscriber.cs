using System;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class BLOBSubscriber
    {
        /// <summary>
        /// Subscribes to messages holding BLOBs.
        /// </summary>
        public static void start(MM.CommandLine commandLine)
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = commandLine.get<string>("gateway-hostname"),
                GatewayPort = commandLine.get<int>("gateway-port"),
                Service = commandLine.get<string>("service"),
            };
            var connection = new MM.Connection(connectionParams);

            // We make subscriptions...
            var s1 = connection.subscribe("A.X", onMessage);
            var s2 = connection.subscribe("A.A", onMessage);
            var s3 = connection.subscribe("A.B", onMessage);
            var s4 = connection.subscribe("C.D", onMessage);

            // We process incoming messages until Enter is pressed...
            Utils.processMessages(connection);
            connection.Dispose();
        }

        /// <summary>
        /// Called when we receive a message with a BLOB.
        /// </summary>
        static int blobCount = 0;
        private static void onMessage(MM.Connection connection, string subject, string replySubject, MM.Message message, object tag)
        {
            try
            {
                blobCount++;
                if (blobCount % 100 == 0)
                {
                    MM.Logger.info(message.toMMListenString(subject, compact: true));
                }
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Exception: {ex.Message}");
            }
        }
    }
}
