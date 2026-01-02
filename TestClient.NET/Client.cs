using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class Client
    {
        /// <summary>
        /// Makes blocking requests to the Server.
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

            // We make requests to add two numbers...
            MM.Logger.info("Sending Service.Add requests");
            var total = 0.0;
            for (var i = 1; i <= 1000; ++i)
            {
                var message = new MM.Message();
                message.addDouble("A", i * 0.5);
                message.addDouble("B", i * 0.5);
                var result = connection.sendRequest("Service.Add", message, 30.0);
                var sum = result.getDouble("SUM");
                total += sum;
            }
            MM.Logger.info($"Total={total}");

            // We process incoming messages until Enter is pressed...
            Utils.processMessages(connection);
            connection.Dispose();
        }
    }
}
