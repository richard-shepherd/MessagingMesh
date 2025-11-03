#pragma once
#include <iostream>

namespace MessagingMesh
{
    class Tests
    {
    // Public methods...
    public:
        // Tests message serialization and deserialization.
        static void messageSerialization();

        // Tests tokenizing strings.
        static void tokenize();

    // Private functions...
    private:

        // Compares values and outputs if they are different.
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
    };
} // namespace


