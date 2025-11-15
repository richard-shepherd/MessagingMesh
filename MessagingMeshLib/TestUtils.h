#pragma once
#include <iostream>
#include <format>

namespace MessagingMesh
{
    namespace TestUtils
    {
        // Manages the lifetime of a test run.
        struct TestRun
        {
            // Constructor.
            TestRun() {}

            // Destructor.
            ~TestRun()
            {
                std::cout << std::endl;
                if (FailureCount == 0)
                {
                    std::cout << "Test run SUCCESS: Passed=" << SuccessCount << ", Failed=" << FailureCount << std::endl;
                }
                else
                {
                    std::cout << "Test run FAILED: Passed=" << SuccessCount << ", Failed=" << FailureCount << std::endl;
                }
            }

            // The number of successes and failures in the current test run.
            int SuccessCount = 0;
            int FailureCount = 0;
        };

        // Logs a message.
        void log(const std::string& message, bool preceedingBlankLine = true);

        // Compares values, and outputs if they are the same or different.
        void assertEqual(TestRun& testRun, unsigned char actual, unsigned char expected);

        // Compares values, and outputs if they are the same or different.
        template <typename T>
        void assertEqual(TestRun& testRun, const T& actual, const T& expected)
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
            std::cout << std::format("{}: expected={}, actual={}", result, expected, actual) << std::endl;
        }

    }  // namespace TestUtils
} // namespace MessagingMesh


