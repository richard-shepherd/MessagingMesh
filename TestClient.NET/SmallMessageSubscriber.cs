using MessagingMeshLib.NET;
using System;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class SmallMessageSubscriber
    {
        /// <summary>
        /// Subscribes to small messages.
        /// </summary>
        public static void start()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "127.0.0.1",
                GatewayPort = 5050,
                Service = "VULCAN",
                ClientID = "SmallMessageSubscriber"
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
        /// Called when we receive a message.
        /// </summary>
        private static void onMessage(Connection connection, string subject, string replySubject, Message message, object tag)
        {
            try
            {
                var value = message.getSignedInt32("#");
                if (value % 1000000 == 0)
                {
                    MM.Logger.info($"Update to {subject}: {value}, tag:{tag}");
                }
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Exception: {ex.Message}");
            }
        }
    }
}
