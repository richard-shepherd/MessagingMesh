#include <iostream>
#include <format>
#include "MessagingMesh.h"
namespace MM = MessagingMesh;


// Outputs messaging-mesh logs to the screen.
void onMessageLogged(MM::Logger::LogLevel logLevel, const std::string& message)
{
    auto time = MM::Utils::getTimeString();
    auto& strLogLevel = MM::Logger::toString(logLevel);
    std::cout << time << ": " << strLogLevel << ": " << message << std::endl;
}

// Publishes data...
void publish()
{
    // We connect to the gateway...
    MM::ConnectionParams connectionParams;
    connectionParams.GatewayHost = "localhost";
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

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Called when we receive a message.
void onMessage(MM::Connection& /*connection*/, const std::string& subject, const std::string& /*replySubject*/, MM::MessagePtr pMessage, void* /*tag*/)
{
    static int count = 0;
    try
    {
        count++;
        auto value = pMessage->getSignedInt32("#");
        if (count % 1000000 == 0)
        {
            //MM::Logger::info(std::format("Messages received: {}", count));
            MM::Logger::info(std::format("Update to {}: {}, count={}", subject, value, count));
        }
    }
    catch (const std::exception& ex)
    {
        MM::Logger::error(std::format("{}: {}", __func__, ex.what()));
    }
}

// Subscribes to data...
void subscribe()
{
    // We connect to the gateway...
    MM::ConnectionParams connectionParams;
    connectionParams.GatewayHost = "localhost";
    connectionParams.GatewayPort = 5050;
    connectionParams.Service = "VULCAN";
    MM::Connection connection(connectionParams);

    // We make subscriptions...
    auto s1 = connection.subscribe("A.X", onMessage);
    auto s2 = connection.subscribe("A.A", onMessage);
    auto s3 = connection.subscribe("A.B", onMessage);
    auto s4 = connection.subscribe("C.D", onMessage);

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Makes blocking requests to the server...
void client()
{
    // We connect to the gateway...
    MM::ConnectionParams connectionParams;
    connectionParams.GatewayHost = "localhost";
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

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Responds to Service.Add requests...
void server()
{
    // We connect to the gateway...
    MM::ConnectionParams connectionParams;
    connectionParams.GatewayHost = "localhost";
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
            connection.sendMessage(replySubject, pReply);
        }
    );

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Main.
int main(int argc, char** argv)
{
    MM::Logger::registerCallback(onMessageLogged);

    if (argc >= 2 && strcmp("-pub", argv[1]) == 0)
    {
        publish();
    }
    else if (argc >= 2 && strcmp("-sub", argv[1]) == 0)
    {
        subscribe();
    }
    else if (argc >= 2 && strcmp("-client", argv[1]) == 0)
    {
        client();
    }
    else if (argc >= 2 && strcmp("-server", argv[1]) == 0)
    {
        server();
    }
    else
    {
        MM::Logger::warn("Usage: TestClient.exe -pub/-sub");
    }
}

