#include <iostream>
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
    for (int32_t i = 1; i <= 50000000; ++i)
    {
        {
            auto pMessage = MM::Message::create();
            pMessage->addField("VALUE", i);
            connection.sendMessage("A.B", pMessage);
        }

        //{
        //    auto pMessage = MM::Message::create();
        //    pMessage->addField("VALUE", i + 10);
        //    connection.sendMessage("C.D", pMessage);
        //}
    }

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

// Subscribes to data...
void subscribe()
{
    // We connect to the gateway...
    MM::Connection connection("localhost", 5050, "VULCAN");

    // We make subscriptions...
    auto s1 = connection.subscribe(
        "A.B",
        [](const std::string& /*subject*/, const std::string& /*replySubject*/, MM::MessagePtr pMessage)
        {
            auto value = pMessage->getField("VALUE")->getSignedInt32();
            if (value % 1000000 == 0)
            {
                MM::Logger::info(MM::Utils::format("Update to A.B: %d", value));
            }
        }
    );
    auto s2 = connection.subscribe(
        "C.D",
        [](const std::string& /*subject*/, const std::string& /*replySubject*/, MM::MessagePtr pMessage)
        {
            auto value = pMessage->getField("VALUE")->getSignedInt32();
            MM::Logger::info(MM::Utils::format("Update to C.D: %d", value));
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
    else
    {
        MM::Logger::warn("Usage: TestClient.exe -pub/-sub");
    }
}

