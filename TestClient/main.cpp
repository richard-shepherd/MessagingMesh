#include <iostream>
#include "MessagingMesh.h"
using namespace MessagingMesh;


// Outputs messaging-mesh logs to the screen.
void onMessageLogged(Logger::LogLevel logLevel, const std::string& message)
{
    auto time = MMUtils::getTimeString();
    std::cout << time << ": " << Logger::toString(logLevel) << ": " << message << std::endl;
}

// Main.
int main(int /*argc*/, char** /*argv*/)
{
    Logger::registerCallback(onMessageLogged);

    // We connect to the gateway...
    Connection connection("localhost", 5050, "VULCAN");

    // We make subscriptions...
    auto s1 = connection.subscribe("A.B", nullptr);
    auto s2 = connection.subscribe("C.D", nullptr);

    // We send updates...
    Logger::info("Sending data");
    for (int32_t i = 1; i <= 5; ++i)
    {
        auto pMessage = Message::create();
        pMessage->addField("VALUE", i);
        connection.sendMessage("A.B", pMessage);
    }

    Logger::info("Press Enter to exit");
    std::cin.get();
}

