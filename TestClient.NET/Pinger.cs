using System.Diagnostics;
using System;
using MM = MessagingMeshLib.NET;
using System.Threading;
using MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class Pinger
    {
        /// <summary>
        /// Runs the pinger.
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

            // We subscribe to pong replies...
            var totalPingMicroseconds = 0.0;
            var sampleSize = 10;
            var count = 0;
            connection.subscribe("PONG", (c, s, rs, m, t) =>
            {
                // We get the ping time...
                var ticks_now = Stopwatch.GetTimestamp();
                var ticks_ping = m.getSignedInt64("TICKS");
                var pingMicroseconds = (ticks_now - ticks_ping) / (double)Stopwatch.Frequency * 1000000.0;

                // We log each sample...
                totalPingMicroseconds += pingMicroseconds;
                count++;
                if (count == sampleSize)
                {
                    var sampleMicroseconds = totalPingMicroseconds / sampleSize;
                    Logger.info($"us={sampleMicroseconds:0.00}");
                    totalPingMicroseconds = 0.0;
                    count = 0;
                }

                Thread.Sleep(100);
                //var start = Stopwatch.GetTimestamp();
                //for (; ; )
                //{
                //    var now = Stopwatch.GetTimestamp();
                //    var us = (now - start) / (double)Stopwatch.Frequency * 1000000.0;
                //    if (us > 10000.0)
                //    {
                //        break;
                //    }
                //}

                // We send the next ping...
                var nextPing = new MM.Message();
                nextPing.addSignedInt64("TICKS", Stopwatch.GetTimestamp());
                connection.sendMessage(nextPing, "PING");
            });

            // We send an initial ping to kick things off...
            var intialPing = new MM.Message();
            intialPing.addSignedInt64("TICKS", Stopwatch.GetTimestamp());
            connection.sendMessage(intialPing, "PING");

            Utils.processMessages(connection);
            connection.Dispose();
        }
    }
}
