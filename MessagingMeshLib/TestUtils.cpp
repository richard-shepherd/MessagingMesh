#include "TestUtils.h"

// Logs a message.
void MessagingMesh::TestUtils::log(const std::string& message, bool preceedingBlankLine)
{
    if (preceedingBlankLine)
    {
        std::cout << std::endl;
    }
    std::cout << message << std::endl;
}
