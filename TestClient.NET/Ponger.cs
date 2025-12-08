using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class Ponger
    {
        /// <summary>
        /// Runs the ponger.
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

            // We subscribe to ping messages, and reply with a pong...
            connection.subscribe("PING", (c, s, rs, m, t) =>
            {
                var message = new MM.Message();
                message.addSignedInt64("TICKS", m.getSignedInt64("TICKS"));
                connection.sendMessage(message, "PONG");
            });

            Utils.processMessages(connection);
            connection.Dispose();
        }

    }
}
