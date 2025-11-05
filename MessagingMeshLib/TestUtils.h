#pragma once
#include <iostream>

namespace MessagingMesh
{
    namespace TestUtils
    {
        // Logs a message.
        void log(const std::string& message, bool preceedingBlankLine = true)
        {
            if (preceedingBlankLine)
            {
                std::cout << std::endl;
            }
            std::cout  << message << std::endl;
        }

        // Compares values, and outputs if they are the same or different.
        template <typename T>
        void assertEqual(const T& actual, const T& expected)
        {
            if (actual == expected)
            {
                std::cout << "PASS: expected=" << expected << ", actual=" << actual << std::endl;
            }
            else
            {
                std::cout << "FAIL: expected=" << expected << ", actual=" << actual << std::endl;
            }
        }
    }  // namespace TestUtils
} // namespace MessagingMesh


