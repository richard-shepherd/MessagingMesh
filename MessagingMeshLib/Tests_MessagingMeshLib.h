#pragma once
#include "TestUtils.h"

namespace MessagingMesh
{
    /// <summary>
    /// Tests for MessagingMeshLib.
    /// </summary>
    class Tests_MessagingMeshLib
    {
    // Public methods...
    public:
        // Runs all tests.
        static void runAll(TestUtils::TestRun& testRun);

        // Tests message serialization and deserialization.
        static void messageSerialization(TestUtils::TestRun& testRun);

        // Tests tokenizing strings.
        static void tokenize(TestUtils::TestRun& testRun);

        // Tests for generating GUIDs.
        static void guids(TestUtils::TestRun& testRun);
    };
} // namespace


