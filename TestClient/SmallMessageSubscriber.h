#pragma once
#include <format>
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Subscribes to data.
class SmallMessageSubscriber
{
public:
    // Runs the subscriber.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        connectionParams.ClientID = "SmallMessageSubscriber";
        MM::Connection connection(connectionParams);

        // We make subscriptions...
        auto s1 = connection.subscribe("A.X", onMessage);
        auto s2 = connection.subscribe("A.A", onMessage);
        auto s3 = connection.subscribe("A.B", onMessage);
        auto s4 = connection.subscribe("C.D", onMessage);

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }

private:
    // Called when we receive a message.
    static void onMessage(MM::Connection& /*connection*/, const std::string& subject, const std::string& /*replySubject*/, MM::MessagePtr pMessage, void* /*tag*/)
    {
        static int count = 0;
        try
        {
            count++;
            auto value = pMessage->getSignedInt32("#");
            if (count % 1000000 == 0)
            {
                MM::Logger::info(std::format("Update to {}: {}, count={}", subject, value, count));
            }
        }
        catch (const std::exception& ex)
        {
            MM::Logger::error(std::format("{}: {}", __func__, ex.what()));
        }
    }
};

