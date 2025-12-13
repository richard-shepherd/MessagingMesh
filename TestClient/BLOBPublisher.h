#pragma once
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Publishes messages holding BLOBs.
class BLOBPublisher
{
public:
    // Runs the BLOB publisher.
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
        const int32_t size = 2000000;
        char* pData = new char[size];
        auto pBLOB = MM::BLOB::create_fromData(pData, size, MM::BLOB::Ownership::TAKE_OWNERSHIP);
        for (int i = 1; i <= 10000; ++i)
        {
            {
                auto message = MM::Message::create();
                message->addBLOB("#", pBLOB);
                message->addSignedInt32("A", i);
                connection.sendMessage(message, "A.B");
                if (i % 3 == 0)
                {
                    Utils::sleep(1);
                }
            }
        }

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }
};

