#pragma once
#include <conio.h>
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
            connection.processMessageQueue(10);

            // We check for Enter...
            if (_kbhit())
            {
                auto key = _getch();
                if (key == 13) break;
            }
        }
    }
};

