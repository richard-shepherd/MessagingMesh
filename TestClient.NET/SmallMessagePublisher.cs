using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class SmallMessagePublisher
    {
        /// <summary>
        /// Publishes small messages.
        /// </summary>
        public static void start()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "127.0.0.1",
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
                    message.addSignedInt32("#", i);
                    connection.sendMessage(message, "A.B");
                }
            }

            // We process incoming messages until Enter is pressed...
            Utils.processMessages(connection);
            connection.Dispose();
        }
    }
}
