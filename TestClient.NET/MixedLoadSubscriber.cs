using System;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class MixedLoadSubscriber
    {
        /// <summary>
        /// Subscribes to messages published by the MixedLoadPublisher.
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
            var s1 = connection.subscribe("MIXED-LOAD.>", onMessage);

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
                if (blobCount % 1000 == 0)
                {
                    var blob = message.getBLOB("DATA");
                    MM.Logger.info($"Update to {subject}: BLOB-size={blob.Length}, tag:{tag}");
                }
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Exception: {ex.Message}");
            }
        }
    }
}
