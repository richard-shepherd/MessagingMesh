#pragma once
#include "SharedAliases.h"
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

        // Tests writing to a reading from a buffer.
        static void buffer(TestUtils::TestRun& testRun);

        // Tests message serialization and deserialization.
        static void messageSerialization(TestUtils::TestRun& testRun);

        // Tests deserializing messages from binary data (serialized from different languages).
        static void deserializedMessages(TestUtils::TestRun& testRun);

        // Tests updating fields in messages.
        static void updateFields(TestUtils::TestRun& testRun);

        // Tests tokenizing strings.
        static void tokenize(TestUtils::TestRun& testRun);

        // Tests for generating GUIDs.
        static void guids(TestUtils::TestRun& testRun);

    // Private functions...
    private:
        // Tests message fields for message serialization tests.
        static void testMessageFields(TestUtils::TestRun& testRun, const ConstMessagePtr& m);

        // Saves a buffer to a file.
        static void saveBuffer(const std::string& filename, const ConstBufferPtr& pBuffer);

        // Loads a buffer from a file.
        static ConstBufferPtr loadBuffer(const std::string& filename);
    };
} // namespace


