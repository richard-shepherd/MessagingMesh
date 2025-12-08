using System.Threading;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class BLOBPublisher
    {
        /// <summary>
        /// Publishes messages holding BLOBs.
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
            const int BLOB_SIZE = 2000000;
            var blob = new byte[BLOB_SIZE];
            for (int i = 1; i <= 10000; ++i)
            {
                {
                    var message = new MM.Message();
                    message.addBLOB("#", blob);
                    message.addSignedInt32("A", i);
                    connection.sendMessage(message, "A.B");
                    if (i % 3 == 0)
                    {
                        Thread.Sleep(1);
                    }
                }
            }

            // We process incoming messages until Enter is pressed...
            Utils.processMessages(connection);
            connection.Dispose();
        }
    }
}
