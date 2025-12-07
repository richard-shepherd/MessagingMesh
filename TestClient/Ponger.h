#pragma once
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Ponger.
class Ponger
{
public:
    // Runs the pinger.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        MM::Connection connection(connectionParams);

        // We subscribe to ping messages, and reply with a pong...
        auto pingSubscription = connection.subscribe("PING", [](auto& c, auto& /*s*/, auto& /*rs*/, auto m, auto /*t*/)
            {
                auto message = MM::Message::create();
                message->addSignedInt64("US---", m->getSignedInt64("US---"));
                c.sendMessage("PONG", message);
            });

        Utils::processMessages(connection);
    }
};

