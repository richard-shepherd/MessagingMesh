#include <mimalloc/mimalloc-new-delete.h>
#include <iostream>
#include <Logger.h>
#include <Utils.h>
#include <UVUtils.h>
#include <CLI/CLI11.hpp>
#include "Gateway.h"
#include "Tests_Gateway.h"
using namespace MessagingMesh;

// Logs messages to the screen.
void onMessageLogged(Logger::LogLevel logLevel, const std::string& message)
{
    auto time = Utils::getTimeString();
    auto& strLogLevel = Logger::toString(logLevel);
    std::cout << time << ": " << strLogLevel << ": " << message << std::endl;
}

// Main.
int main(int argc, char** argv)
{
    // Command-line parsing...
    CLI::App app("Gateway");
    argv = app.ensure_utf8(argv);
    bool runTests = false;
    int port;
    app.add_flag("-t,--test", runTests, "Runs tests");
    app.add_option("-p,--port", port, "Listening port")->default_val(5050);
    CLI11_PARSE(app, argc, argv);

    if (runTests)  
    {
        // We run tests...
        Tests_Gateway::runAll();
    }
    else
    {
        // We run the gateway.

        // We set the thread name...
        UVUtils::setThreadName("MAIN");

        // We set up logging...
        Logger::registerCallback(onMessageLogged);

        // We log the mimalloc version...
        Logger::info(std::format("Using mimalloc version {}", mi_version()));

        // We run the gateway...
        Gateway gateway(port);

        Logger::info("Press Enter to exit");
        std::cin.get();
    }
}

