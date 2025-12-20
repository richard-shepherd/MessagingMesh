#pragma once
#include <format>
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Subscribes to messages holding BLOBs.
class BLOBSubscriber
{
public:
    // Runs the BLOB subscriber.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        MM::Connection connection(connectionParams);

        // We make subscriptions...
        blobCount = 0;
        auto s1 = connection.subscribe("A.X", onMessage);
        auto s2 = connection.subscribe("A.A", onMessage);
        auto s3 = connection.subscribe("A.B", onMessage);
        auto s4 = connection.subscribe("C.D", onMessage);

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }

private:
    /// <summary>
    /// Called when we receive a message with a BLOB.
    /// </summary>
    inline static int blobCount = 0;
    static void onMessage(MM::Connection& /*connection*/, const std::string& subject, const std::string& /*replySubject*/, MM::MessagePtr pMessage, void* /*tag*/)
    {
        try
        {
            blobCount++;
            if (blobCount % 100 == 0)
            {
                auto pBLOB = pMessage->getBLOB("#");
                auto a = pMessage->getSignedInt32("A");
                MM::Logger::info(std::format("Update to {}: A={}, BLOB-size={}", subject, a, pBLOB->getLength()));
            }
        }
        catch (const std::exception& ex)
        {
            MM::Logger::error(std::format("{}: {}", __func__, ex.what()));
        }
    }
};

