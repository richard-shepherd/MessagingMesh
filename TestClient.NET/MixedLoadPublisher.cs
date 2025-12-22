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
            createMessageInfos(messageInfos, "TINY",           10, 0.00, 0.30);
            createMessageInfos(messageInfos, "SMALL",         100, 0.30, 0.60);
            createMessageInfos(messageInfos, "MEDIUM",       1000, 0.60, 0.80);
            createMessageInfos(messageInfos, "LARGE",       10000, 0.80, 0.90);
            createMessageInfos(messageInfos, "VERY-LARGE", 100000, 0.90, 0.95);
            createMessageInfos(messageInfos, "HUGE",      1000000, 0.95, 1.00);

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
        /// Creates a number of message-infos and adds them to the collection.
        /// </summary>
        private static void createMessageInfos(List<MessageInfo> messageInfos, string baseSubject, int baseSize, double probabilityLowerBound, double probabilityUpperBound)
        {
            const int MESSAGES_TO_CREATE = 10;

            var probabilityInterval = probabilityUpperBound - probabilityLowerBound;
            var probabilityStep = probabilityInterval / MESSAGES_TO_CREATE;
            for (var i = 0; i < MESSAGES_TO_CREATE; ++i)
            {
                var subject = $"{baseSubject}-{i}";
                var size = baseSize + baseSize * i / MESSAGES_TO_CREATE;
                var lowerBound = probabilityLowerBound + i * probabilityStep;
                var upperBound = lowerBound + probabilityStep;
                messageInfos.Add(createMessageInfo(subject, size, lowerBound, upperBound));
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
