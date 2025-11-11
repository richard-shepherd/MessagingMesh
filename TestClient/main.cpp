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
    MM::Connection connection("localhost", 5050, "VULCAN");

    // We send updates...
    MM::Logger::info("Sending data");
    for (int32_t i = 1; i <= 10000000; ++i)
    {
        {
            auto pMessage = MM::Message::create();
            pMessage->addField("#", i);
            connection.sendMessage("A.B", pMessage);
        }

        //{
        //    auto pMessage = MM::Message::create();
        //    pMessage->addField("#", i + 10);
        //    connection.sendMessage("C.D", pMessage);
        //}
    }

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Called when we receive a message.
void onMessage(const std::string& subject, const std::string& /*replySubject*/, MM::MessagePtr pMessage)
{
    try
    {
        auto value = pMessage->getField("#")->getSignedInt32();
        if (value % 1000000 == 0)
        {
            MM::Logger::info(MM::Utils::format("Update to %s: %d", subject.c_str(), value));
        }
    }
    catch (const std::exception& ex)
    {
        MM::Logger::error(MM::Utils::format("%s: %s", __func__, ex.what()));
    }
}

// Subscribes to data...
void subscribe()
{
    // We connect to the gateway...
    MM::Connection connection("localhost", 5050, "VULCAN");

    // We make subscriptions...
    auto s1 = connection.subscribe("A.B", onMessage);
    auto s2 = connection.subscribe("C.D", onMessage);

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Makes blocking requests to the server...
void client()
{
    // We connect to the gateway...
    MM::Connection connection("localhost", 5050, "VULCAN");

    // We make requests to add two numbers...
    MM::Logger::info("Sending Service.Add requests");
    auto total = 0.0;
    for (auto i = 1; i <= 1000; ++i)
    {
        auto pMessage = MM::Message::create();
        pMessage->addField("A", i * 0.5);
        pMessage->addField("B", i * 0.5);
        auto pResult = connection.sendRequest("Service.Add", pMessage, 30.0);
        auto sum = pResult->getField("SUM")->getDouble();
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
    MM::Connection connection("localhost", 5050, "VULCAN");

    // We subscribe to requests...
    auto s1 = connection.subscribe(
        "Service.Add",
        [&connection](const std::string& /*subject*/, const std::string& replySubject, MM::MessagePtr pMessage)
        {
            // We get the values from the request and add them...
            auto a = pMessage->getField("A")->getDouble();
            auto b = pMessage->getField("B")->getDouble();
            auto sum = a + b;

            // We reply with a message holding the sum...
            auto pReply = MM::Message::create();
            pReply->addField("SUM", sum);
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

