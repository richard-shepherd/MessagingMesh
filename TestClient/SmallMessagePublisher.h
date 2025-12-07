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
        MM::Connection connection(connectionParams);

        // We send updates...
        MM::Logger::info("Sending data");
        //const int BLOB_LENGTH = 10;
        //char blob[BLOB_LENGTH];
        //auto pBLOB = MM::BLOB::create_fromData(blob, BLOB_LENGTH, MM::BLOB::Ownership::HOLD_REFERENCE);
        for (int32_t i = 1; i <= 10000000; ++i)
        {
            {
                auto pMessage = MM::Message::create();
                pMessage->addSignedInt32("#", i);
                //pMessage->addBLOB("#", pBLOB);
                connection.sendMessage("A.B", pMessage);
            }
        }

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }
};

