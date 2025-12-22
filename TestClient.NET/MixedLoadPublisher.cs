using MessagingMeshLib.NET;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using MM = MessagingMeshLib.NET;

namespace TestClient.NET
{
    internal class MixedLoadPublisher
    {
        /// <summary>
        /// Publishes mixed sized messages at random intervals.
        /// </summary>
        public static void start()
        {
            // We connect to the gateway...
            var connectionParams = new MM.ConnectionParams
            {
                GatewayHost = "127.0.0.1",
                GatewayPort = 5050,
                Service = "VULCAN",
                ClientID = "MixedLoadPublisher"
            };
            var connection = new MM.Connection(connectionParams);

            // We create messages of various sizes and sore them in a 
            var messageInfos = new List<MessageInfo>();
            messageInfos.Add(createMessageInfo("TINY-1", 10, 0, 0.1));
            messageInfos.Add(createMessageInfo("TINY-2", 12, 0.1, 0.2));
            messageInfos.Add(createMessageInfo("TINY-3", 13, 0.2, 0.3));
            messageInfos.Add(createMessageInfo("SMALL-1", 100, 0.3, 0.4));
            messageInfos.Add(createMessageInfo("SMALL-2", 120, 0.4, 0.5));
            messageInfos.Add(createMessageInfo("SMALL-3", 130, 0.5, 0.6));
            messageInfos.Add(createMessageInfo("MEDIUM-1", 1000, 0.6, 0.66));
            messageInfos.Add(createMessageInfo("MEDIUM-2", 1000, 0.66, 0.72));
            messageInfos.Add(createMessageInfo("MEDIUM-3", 1000, 0.72, 0.8));
            messageInfos.Add(createMessageInfo("LARGE-1", 10000, 0.8, 0.83));
            messageInfos.Add(createMessageInfo("LARGE-2", 10000, 0.83, 0.86));
            messageInfos.Add(createMessageInfo("LARGE-3", 10000, 0.86, 0.9));
            messageInfos.Add(createMessageInfo("VERY-LARGE-1", 100000, 0.9, 0.91));
            messageInfos.Add(createMessageInfo("VERY-LARGE-2", 100000, 0.91, 0.93));
            messageInfos.Add(createMessageInfo("VERY-LARGE-3", 100000, 0.93, 0.95));
            messageInfos.Add(createMessageInfo("HUGE-1", 1000000, 0.95, 0.96));
            messageInfos.Add(createMessageInfo("HUGE-2", 1000000, 0.96, 0.98));
            messageInfos.Add(createMessageInfo("HUGE-3", 1000000, 0.98, 1.0));

            // We send updates...
            var rnd = new Random();
            MM.Logger.info("Sending data");
            for(; ;)
            {
                // We send a random number of messages...
                var messagesToSend = rnd.Next(10, 100);
                for(var i=0; i<messagesToSend; ++i)
                {
                    // We get a message depending on the probabilities...
                    var r = rnd.NextDouble();
                    var messageInfo = messageInfos.Where(x => x.ProbabilityLowerBound <= r && x.ProbabilityUpperBound > r).SingleOrDefault();
                    if(messageInfo != null)
                    {
                        connection.sendMessage(messageInfo.Message, messageInfo.Subject);
                    }
                }

                // We sleep for a random time...
                var intervalMS = rnd.Next(10, 100);
                Thread.Sleep(intervalMS);
            }
        }

        /// <summary>
        /// Creates a MessageInfo holding a message of the size requested.
        /// </summary>
        private static MessageInfo createMessageInfo(string subject, int size, double probabilityLowerBound, double probabilityUpperBound)
        {
            var messageInfo = new MessageInfo();
            messageInfo.Message = new MM.Message();
            messageInfo.Message.addBLOB("DATA", new byte[size]);
            messageInfo.Subject = $"MIXED-LOAD.{subject}";
            messageInfo.ProbabilityLowerBound = probabilityLowerBound;
            messageInfo.ProbabilityUpperBound = probabilityUpperBound;
            return messageInfo;
        }

        // A message, subject and probability of being sent.
        private class MessageInfo
        {
            public double ProbabilityLowerBound { get; set; }
            public double ProbabilityUpperBound { get; set; }
            public string Subject { get; set; }
            public MM.Message Message { get; set; }
        }
    }
}
