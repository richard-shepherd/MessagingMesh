#pragma once
#include <iostream>

namespace MessagingMesh
{
    namespace TestUtils
    {
        // Compares values, and outputs if they are the same or different.
        template <typename T>
        static void assertEqual(const T& actual, const T& expected)
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


