#pragma once
#include <conio.h>
#include <format>
#include <MessagingMesh.h>
namespace MM = MessagingMesh;

// Utils for tests.
class Utils
{
public:

    // Processes messages until Enter is pressed.
    static void processMessages(MM::Connection& connection)
    {
        MM::Logger::info("Press Enter to exit");
        for (; ; )
        {
            // We process messages...
            //auto info = connection.processMessageQueue(10, 100);
            auto info = connection.processMessageQueue(10);
            //MM::Logger::info(std::format("P={}, Q={}", info.MessagesProcessed, info.QueueSize));

            // We check for Enter...
            if (_kbhit())
            {
                auto key = _getch();
                if (key == 13) break;
            }
        }
    }

    // Returns the number of microseconds since the epoch.
    static int64_t microsecondsSinceEpoch()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    // Sleeps for the time specified.
    static void sleep(int milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
};

