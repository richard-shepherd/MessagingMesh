#pragma once
#include <iostream>

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
        template <typename T>
        void assertEqual(TestRun& testRun, const T& actual, const T& expected)
        {
            if (actual == expected)
            {
                std::cout << "PASS: expected=" << expected << ", actual=" << actual << std::endl;
                testRun.SuccessCount++;
            }
            else
            {
                std::cout << "FAIL: expected=" << expected << ", actual=" << actual << std::endl;
                testRun.FailureCount++;
            }
        }
    }  // namespace TestUtils
} // namespace MessagingMesh


