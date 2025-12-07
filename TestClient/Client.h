#pragma once
#include <format>
#include <MessagingMesh.h>
#include "Utils.h"
namespace MM = MessagingMesh;

// Makes blocking requests to the server.
class Client
{
public:
    // Runs the client.
    static void start()
    {
        // We connect to the gateway...
        MM::ConnectionParams connectionParams;
        connectionParams.GatewayHost = "127.0.0.1";
        connectionParams.GatewayPort = 5050;
        connectionParams.Service = "VULCAN";
        MM::Connection connection(connectionParams);

        // We make requests to add two numbers...
        MM::Logger::info("Sending Service.Add requests");
        auto total = 0.0;
        for (auto i = 1; i <= 1000; ++i)
        {
            auto pMessage = MM::Message::create();
            pMessage->addDouble("A", i * 0.5);
            pMessage->addDouble("B", i * 0.5);
            auto pResult = connection.sendRequest("Service.Add", pMessage, 30.0);
            auto sum = pResult->getDouble("SUM");
            total += sum;
        }
        MM::Logger::info(std::format("Total={}", total));

        // We process incoming messages until Enter is pressed...
        Utils::processMessages(connection);
    }
};

