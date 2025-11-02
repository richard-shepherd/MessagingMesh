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

// Main.
int main(int /*argc*/, char** /*argv*/)
{
    MM::Logger::registerCallback(onMessageLogged);

    // We connect to the gateway...
    MM::Connection connection("localhost", 5050, "VULCAN");

    // We make subscriptions...
    auto s1 = connection.subscribe("A.B", nullptr);
    auto s2 = connection.subscribe("C.D", nullptr);

    // We send updates...
    MM::Logger::info("Sending data");
    for (int32_t i = 1; i <= 5; ++i)
    {
        auto pMessage = MM::Message::create();
        pMessage->addField("VALUE", i);
        connection.sendMessage("A.B", pMessage);
    }

    MM::Logger::info("Press Enter to exit");
    std::cin.get();
}

