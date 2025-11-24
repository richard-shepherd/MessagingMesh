using MessagingMeshLib.NET;
using System;
using System.Runtime.Remoting.Messaging;
using System.Threading;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class TestClient
    {

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
        private static void processMessages(Connection connection)
        {
            MM.Logger.info("Press Enter to exit");
            for (; ; )
            {
                // We process messages...
                connection.processMessageQueue(millisecondsTimeout: 10);

                // We check for Enter...
                if (Console.KeyAvailable)
                {
                    var key = Console.ReadKey(intercept: true);
                    if (key.Key == ConsoleKey.Enter) break;
                }
            }
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

        /// <summary>
        /// Called when we receive a message with a BLOB.
        /// </summary>
        static int blobCount = 0;
        private static void onMessageBLOB(Connection connection, string subject, string replySubject, Message message, object tag)
        {
            try
            {
                blobCount++;
                var blob = message.getBLOB("#");
                if (blobCount % 1 == 0)
                {
                    MM.Logger.info($"Update to {subject}: BLOB-size={blob.Length}, tag:{tag}");
                }
            }
            catch (Exception ex)
            {
                MM.Logger.error($"Exception: {ex.Message}");
            }
        }

        /// <summary>
        /// Subscribes to messages.
        /// </summary>
        private static void subscribe()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We make subscriptions...
            var s1 = connection.subscribe("A.X", onMessage);
            var s2 = connection.subscribe("A.A", onMessage);
            var s3 = connection.subscribe("A.B", onMessage);
            var s4 = connection.subscribe("C.D", onMessage);

            // We process incoming messages until Enter is pressed...
            processMessages(connection);
            connection.Dispose();
        }

        /// <summary>
        /// Subscribes to messages holding BLOBs.
        /// </summary>
        private static void subscribeBLOB()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We make subscriptions...
            var s1 = connection.subscribe("A.X", onMessageBLOB);
            var s2 = connection.subscribe("A.A", onMessageBLOB);
            var s3 = connection.subscribe("A.B", onMessageBLOB);
            var s4 = connection.subscribe("C.D", onMessageBLOB);

            // We process incoming messages until Enter is pressed...
            processMessages(connection);
            connection.Dispose();
        }

        /// <summary>
        /// Publishes messages.
        /// </summary>
        private static void publish()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
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
                    connection.sendMessage("A.B", message);
                }
            }

            // We process incoming messages until Enter is pressed...
            processMessages(connection);
            connection.Dispose();
        }

        /// <summary>
        /// Publishes messages holding BLOBs.
        /// </summary>
        private static void publishBLOB()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We send updates...
            MM.Logger.info("Sending data");
            const int BLOB_SIZE = 20000000;
            var blob = new byte[BLOB_SIZE];
            for (int i = 1; i <= 10000000; ++i)
            {
                {
                    var message = new MM.Message();
                    message.addBLOB("#", blob);
                    connection.sendMessage("A.B", message);
                    if(i % 1 == 0)
                    {
                        Thread.Sleep(1000);
                    }
                }
            }

            // We process incoming messages until Enter is pressed...
            processMessages(connection);
            connection.Dispose();
        }

        // Makes blocking requests to the server...
        static void client()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
                Service = "VULCAN"
            };
            var connection = new MM.Connection(connectionParams);

            // We make requests to add two numbers...
            MM.Logger.info("Sending Service.Add requests");
            var total = 0.0;
            for (var i = 1; i <= 1000; ++i)
            {
                var message =  new Message();
                message.addDouble("A", i * 0.5);
                message.addDouble("B", i * 0.5);
                var result = connection.sendRequest("Service.Add", message, 30.0);
                var sum = result.getDouble("SUM");
                total += sum;
            }
            MM.Logger.info($"Total={total}");

            // We process incoming messages until Enter is pressed...
            processMessages(connection);
            connection.Dispose();
        }

        // Responds to Service.Add requests...
        static void server()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "localhost",
                GatewayPort = 5050,
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
                    var reply = new Message();
                    reply.addDouble("SUM", sum);
                    connection.sendMessage(rs, reply);
                }
            );

            // We process incoming messages until Enter is pressed...
            processMessages(connection);
            connection.Dispose();
        }

        /// <summary>
        /// Main.
        /// </summary>
        static void Main(string[] args)
        {
            // We name the thread...
            Thread.CurrentThread.Name = "MAIN";

            // We connect to the logger...
            MM.Logger.registerCallback(onMessagingMeshLogMessage);

            if (args.Length > 0 && args[0] == "-sub")
            {
                subscribe();
            }
            else if(args.Length > 0 && args[0] == "-pub")
            {
                publish();
            }
            else if (args.Length > 0 && args[0] == "-sub-blob")
            {
                subscribeBLOB();
            }
            else if(args.Length > 0 && args[0] == "-pub-blob")
            {
                publishBLOB();
            }
            else if(args.Length > 0 && args[0] == "-client")
            {
                client();
            }
            else if(args.Length > 0 && args[0] == "-server")
            {
                server();
            }
            else
            {
                MM.Logger.error("Usage: TestClient.NET.exe -sub/-pub");
            }
        }
    }
}
