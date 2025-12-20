#pragma once
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Publishes messages.
class SmallMessagePublisher
{
public:
    // Runs the publisher.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        connectionParams.ClientID = "SmallMessagePublisher";
        MM::Connection connection(connectionParams);

        // We send updates...
        MM::Logger::info("Sending data");
        for (int32_t i = 1; i <= 10000000; ++i)
        {
            {
                auto pMessage = MM::Message::create();
                pMessage->addSignedInt32("#", i);
                connection.sendMessage(pMessage, "A.B");
            }
        }

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }
};

