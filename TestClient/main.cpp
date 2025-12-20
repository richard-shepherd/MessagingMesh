#include <mimalloc/mimalloc-new-delete.h>
#include <iostream>
#include <format>
#include <MessagingMesh.h>
#include "SmallMessagePublisher.h"
#include "SmallMessageSubscriber.h"
#include "BLOBPublisher.h"
#include "BLOBSubscriber.h"
#include "Client.h"
#include "Server.h"
#include "Pinger.h"
#include "Ponger.h"
namespace MM = MessagingMesh;

// Outputs messaging-mesh logs to the screen.
void onMessageLogged(MM::Logger::LogLevel logLevel, const std::string& message)
{
    auto time = MM::Utils::getTimeString();
    auto& strLogLevel = MM::Logger::toString(logLevel);
    std::cout << std::format("{}: {}: {}", time, strLogLevel, message) << std::endl;
}

// Main.
int main(int argc, char** argv)
{
    MM::Logger::registerCallback(onMessageLogged);
    MM::Logger::info(std::format("MessagingMesh version={}, mimalloc version={}", MM::Connection::getVersion(), mi_version()));
    try
    {
        if (argc >= 2 && strcmp("-pub", argv[1]) == 0)
        {
            SmallMessagePublisher::start();
        }
        else if (argc >= 2 && strcmp("-sub", argv[1]) == 0)
        {
            SmallMessageSubscriber::start();
        }
        else if (argc >= 2 && strcmp("-pub-blob", argv[1]) == 0)
        {
            BLOBPublisher::start();
        }
        else if (argc >= 2 && strcmp("-sub-blob", argv[1]) == 0)
        {
            BLOBSubscriber::start();
        }
        else if (argc >= 2 && strcmp("-client", argv[1]) == 0)
        {
            Client::start();
        }
        else if (argc >= 2 && strcmp("-server", argv[1]) == 0)
        {
            Server::start();
        }
        else if (argc >= 2 && strcmp("-ping", argv[1]) == 0)
        {
            Pinger::start();
        }
        else if (argc >= 2 && strcmp("-pong", argv[1]) == 0)
        {
            Ponger::start();
        }
        else
        {
            MM::Logger::warn("Usage: TestClient.exe -pub/-sub, -client/-server, -ping/-pong");
        }
    }
    catch (const std::exception& ex)
    {
        MM::Logger::error(ex.what());
    }
    catch (...)
    {
        MM::Logger::error("Unexpected exception");
    }
}

