#pragma once
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Responds to Service.Add requests.
class Server
{
public:
    // Runs the server.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        MM::Connection connection(connectionParams);

        // We subscribe to requests...
        auto s1 = connection.subscribe(
            "Service.Add",
            [](MM::Connection& connection, const std::string& /*subject*/, const std::string& replySubject, MM::MessagePtr pMessage, void* /*tag*/)
            {
                // We get the values from the request and add them...
                auto a = pMessage->getDouble("A");
                auto b = pMessage->getDouble("B");
                auto sum = a + b;

                // We reply with a message holding the sum...
                auto pReply = MM::Message::create();
                pReply->addDouble("SUM", sum);
                connection.sendMessage(pReply, replySubject);
            }
        );

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }
};

