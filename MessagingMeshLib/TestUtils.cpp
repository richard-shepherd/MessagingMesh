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

// Compares values, and outputs if they are the same or different.
void MessagingMesh::TestUtils::assertEqual(TestRun& testRun, unsigned char actual, unsigned char expected)
{
    std::string result = "PASS";
    if (actual == expected)
    {
        testRun.SuccessCount++;
    }
    else
    {
        result = "FAIL";
        testRun.FailureCount++;
    }
    std::cout << std::format("{}: expected={:#x}, actual={:#x}", result, expected, actual) << std::endl;
}
