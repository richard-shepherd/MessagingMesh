#include <iostream>
#include "Logger.h"
#include "Gateway.h"
#include "Utils.h"
#include "UVUtils.h"
using namespace MessagingMesh;

// Logs messages to the screen.
void onMessageLogged(Logger::LogLevel logLevel, const std::string& message)
{
    auto time = Utils::getTimeString();
    auto& strLogLevel = Logger::toString(logLevel);
    std::cout << time << ": " << strLogLevel << ": " << message << std::endl;
}

// Main.
int main(int /*argc*/, char** /*argv*/)
{
    // We set the thread name...
    UVUtils::setThreadName("MAIN");

    // We set up logging...
    Logger::registerCallback(onMessageLogged);

    // We run the gateway...
    Gateway gateway(5050);

    Logger::info("Press Enter to exit");
    std::cin.get();
}

