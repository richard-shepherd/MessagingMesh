#include "Tests_MessagingMeshLib.h"
#include "TestUtils.h"
#include "Message.h"
#include "Field.h"
#include "BLOB.h"
#include "Buffer.h"
#include "MMUtils.h"
using namespace MessagingMesh;
using namespace MessagingMesh::TestUtils;

// Runs all tests.
void Tests_MessagingMeshLib::runAll(TestUtils::TestRun& testRun)
{
    buffer(testRun);
    messageSerialization(testRun);
    tokenize(testRun);
    guids(testRun);
}

// Tests writing to a reading from a buffer.
void Tests_MessagingMeshLib::buffer(TestUtils::TestRun& testRun)
{
    TestUtils::log("Buffer int8...");
    {
        auto pBuffer = Buffer::create();
        pBuffer->write_uint8(0x2e);
        auto buffer = pBuffer->getBuffer();
        assertEqual(testRun, buffer[0], 0x05);
        assertEqual(testRun, buffer[1], 0x00);
        assertEqual(testRun, buffer[2], 0x00);
        assertEqual(testRun, buffer[3], 0x00);
        assertEqual(testRun, buffer[4], 0x2e);
    }

    TestUtils::log("Buffer int32...");
    {
        auto pBuffer = Buffer::create();
        pBuffer->write_int32(0x12345678);
        auto buffer = pBuffer->getBuffer();
        assertEqual(testRun, buffer[0], 0x08);
        assertEqual(testRun, buffer[1], 0x00);
        assertEqual(testRun, buffer[2], 0x00);
        assertEqual(testRun, buffer[3], 0x00);
        assertEqual(testRun, buffer[4], 0x78);
        assertEqual(testRun, buffer[5], 0x56);
        assertEqual(testRun, buffer[6], 0x34);
        assertEqual(testRun, buffer[7], 0x12);
    }

    TestUtils::log("Buffer double...");
    {
        auto pBuffer = Buffer::create();
        pBuffer->write_double(3.14);
        auto buffer = pBuffer->getBuffer();
        assertEqual(testRun, buffer[0], 0x0c);
        assertEqual(testRun, buffer[1], 0x00);
        assertEqual(testRun, buffer[2], 0x00);
        assertEqual(testRun, buffer[3], 0x00);
        assertEqual(testRun, buffer[4], 0x1f);
        assertEqual(testRun, buffer[5], 0x85);
        assertEqual(testRun, buffer[6], 0xeb);
        assertEqual(testRun, buffer[7], 0x51);
        assertEqual(testRun, buffer[8], 0xb8);
        assertEqual(testRun, buffer[9], 0x1e);
        assertEqual(testRun, buffer[10], 0x09);
        assertEqual(testRun, buffer[11], 0x40);
    }

    TestUtils::log("Buffer string...");
    {
        auto pBuffer = Buffer::create();
        pBuffer->write_string("hello");
        auto buffer = pBuffer->getBuffer();

        // Size...
        assertEqual(testRun, buffer[0], 0x0d);
        assertEqual(testRun, buffer[1], 0x00);
        assertEqual(testRun, buffer[2], 0x00);
        assertEqual(testRun, buffer[3], 0x00);

        // String size...
        assertEqual(testRun, buffer[4], 0x05);
        assertEqual(testRun, buffer[5], 0x00);
        assertEqual(testRun, buffer[6], 0x00);
        assertEqual(testRun, buffer[7], 0x00);

        // String data...
        assertEqual(testRun, buffer[8], 'h');
        assertEqual(testRun, buffer[9], 'e');
        assertEqual(testRun, buffer[10], 'l');
        assertEqual(testRun, buffer[11], 'l');
        assertEqual(testRun, buffer[12], 'o');
    }
}

// Tests message serialization and deserialization.
void Tests_MessagingMeshLib::messageSerialization(TestRun& testRun)
{
    TestUtils::log("Message serialization...");

    std::string s = "hello, world!";
    int32_t i32_1 = 123456;
    int32_t i32_2 = -123456;
    uint32_t ui32 = 3123456789;
    int64_t i64_1 = 4123456789123456789;
    int64_t i64_2 = -4123456789123456789;
    uint64_t ui64 = 11123456789123456789;
    double d = 123.456;
    bool b1 = true;
    bool b2 = false;
    unsigned char blob[6] = {1, 2, 3, 230, 240, 250 };
    auto pBLOB = BLOB::create_fromData(blob, 6, BLOB::Ownership::TAKE_COPY);

    // We create a message holding all the types...
    auto m = Message::create();
    m->addString("s", s);
    m->addSignedInt32("i32_1", i32_1);
    m->addSignedInt32("i32_2", i32_2);
    m->addUnsignedInt32("ui32", ui32);
    m->addSignedInt64("i64_1", i64_1);
    m->addSignedInt64("i64_2", i64_2);
    m->addUnsignedInt64("ui64", ui64);
    m->addDouble("d", d);
    m->addBool("b1", b1);
    m->addBool("b2", b2);
    m->addBLOB("blob", pBLOB);

    // We create a sub-message and add it...
    auto sm = Message::create();
    sm->addString("s", s);
    sm->addDouble("d", d);
    m->addMessage("sm", sm);

    // We serialize the message to a buffer...
    auto buffer = Buffer::create();
    m->serialize(*buffer);

    // We deserialize to a new message...
    buffer->resetPosition();
    auto m2 = Message::create();
    m2->deserialize(*buffer);

    // We check the values...
    assertEqual(testRun, m2->getString("s"), s);
    assertEqual(testRun, m2->getSignedInt32("i32_1"), i32_1);
    assertEqual(testRun, m2->getSignedInt32("i32_2"), i32_2);
    assertEqual(testRun, m2->getUnsignedInt32("ui32"), ui32);
    assertEqual(testRun, m2->getSignedInt64("i64_1"), i64_1);
    assertEqual(testRun, m2->getSignedInt64("i64_2"), i64_2);
    assertEqual(testRun, m2->getUnsignedInt64("ui64"), ui64);
    assertEqual(testRun, m2->getDouble("d"), d);
    assertEqual(testRun, m2->getBool("b1"), b1);
    assertEqual(testRun, m2->getBool("b2"), b2);

    // The BLOB...
    auto pBLOB2 = m2->getBLOB("blob");
    auto blob2 = (unsigned char*)pBLOB2->getData();
    assertEqual(testRun, pBLOB2->getLength(), 6);
    assertEqual(testRun, blob2[0], 1);
    assertEqual(testRun, blob2[1], 2);
    assertEqual(testRun, blob2[2], 3);
    assertEqual(testRun, blob2[3], 230);
    assertEqual(testRun, blob2[4], 240);
    assertEqual(testRun, blob2[5], 250);

    // The sub-message...
    auto sm2 = m2->getMessage("sm");
    assertEqual(testRun, sm2->getString("s"), s);
    assertEqual(testRun, sm2->getDouble("d"), d);
}


// Tests tokenizing strings.
void Tests_MessagingMeshLib::tokenize(TestRun& testRun)
{
    TestUtils::log("Tokenize 'A.B.C'");
    {
        std::string s1 = "A.B.C";
        auto tokens = MMUtils::tokenize(s1, '.');
        assertEqual(testRun, tokens.size(), (size_t)3);
        assertEqual(testRun, tokens[0], Token("A"));
        assertEqual(testRun, tokens[1], Token("B"));
        assertEqual(testRun, tokens[2], Token("C"));
    }

    TestUtils::log("Tokenize 'Hello.World.123'");
    {
        std::string s1 = "Hello.World.123";
        auto tokens = MMUtils::tokenize(s1, '.');
        assertEqual(testRun, tokens.size(), (size_t)3);
        assertEqual(testRun, tokens[0], Token("Hello"));
        assertEqual(testRun, tokens[1], Token("World"));
        assertEqual(testRun, tokens[2], Token("123"));
    }

    TestUtils::log("Tokenize ''");
    {
        std::string s1 = "";
        auto tokens = MMUtils::tokenize(s1, '.');
        assertEqual(testRun, tokens.size(), (size_t)0);
    }

    TestUtils::log("Tokenize 'A..B'");
    {
        std::string s1 = "A..B";
        auto tokens = MMUtils::tokenize(s1, '.');
        assertEqual(testRun, tokens.size(), (size_t)3);
        assertEqual(testRun, tokens[0], Token("A"));
        assertEqual(testRun, tokens[1], Token(""));
        assertEqual(testRun, tokens[2], Token("B"));
    }

    TestUtils::log("Tokenize '...'");
    {
        std::string s1 = "...";
        auto tokens = MMUtils::tokenize(s1, '.');
        assertEqual(testRun, tokens.size(), (size_t)3);
        assertEqual(testRun, tokens[0], Token(""));
        assertEqual(testRun, tokens[1], Token(""));
        assertEqual(testRun, tokens[2], Token(""));
    }
}

// Tests for generating GUIDs.
void Tests_MessagingMeshLib::guids(TestUtils::TestRun& testRun)
{
    TestUtils::log("GUIDs...");
    auto guid1 = MMUtils::createGUID();
    assertEqual(testRun, guid1.size(), (size_t)24);

    auto guid2 = MMUtils::createGUID();
    assertEqual(testRun, guid2.size(), (size_t)24);

    assertEqual(testRun, guid1 == guid2, false);
}
