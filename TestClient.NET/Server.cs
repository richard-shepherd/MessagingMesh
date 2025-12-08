using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class Server
    {
        /// <summary>
        /// Responds to Service.Add requests from the Client.
        /// </summary>
        public static void start()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "127.0.0.1",
                GatewayPort = 5051,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We subscribe to requests...
            var s1 = connection.subscribe(
                "Service.Add",
                (c, s, rs, m, t) =>
                {
                    // We get the values from the request and add them...
                    var a = m.getDouble("A");
                    var b = m.getDouble("B");
                    var sum = a + b;

                    // We reply with a message holding the sum...
                    var reply = new MM.Message();
                    reply.addDouble("SUM", sum);
                    connection.sendMessage(reply, rs);
                }
            );

            // We process incoming messages until Enter is pressed...
            Utils.processMessages(connection);
            connection.Dispose();
        }
    }
}
