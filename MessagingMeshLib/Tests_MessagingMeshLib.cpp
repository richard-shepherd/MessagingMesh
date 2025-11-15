#include "Tests_MessagingMeshLib.h"
#include "TestUtils.h"
#include "Message.h"
#include "Field.h"
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
        pBuffer->write_int8(0x2e);
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
}

// Tests message serialization and deserialization.
void Tests_MessagingMeshLib::messageSerialization(TestRun& testRun)
{
    TestUtils::log("Message serialization...");

    std::string name = "Charles";
    double age = 47.5;
    int houseNumber = 3;
    std::string street = "London Road";
    std::string city = "Bristol";

    // We create a message...
    auto pPerson = Message::create();
    pPerson->addField("NAME", name);
    pPerson->addField("AGE", age);

    // We add a sub-message...
    auto pAddress = Message::create();
    pAddress->addField("HOUSE-NUMBER", houseNumber);
    pAddress->addField("STREET", street);
    pAddress->addField("CITY", city);
    pPerson->addField("ADDRESS", pAddress);

    // We serialize the message...
    auto pBuffer = Buffer::create();
    pPerson->serialize(*pBuffer);

    // We deserialize the message...
    pBuffer->resetPosition();
    auto pResult = Message::create();
    pResult->deserialize(*pBuffer);

    assertEqual(testRun, pResult->getField("NAME")->getString(), name);
    assertEqual(testRun, pResult->getField("AGE")->getDouble(), age);

    auto pAddressResult = pResult->getField("ADDRESS")->getMessage();
    assertEqual(testRun, pAddressResult->getField("HOUSE-NUMBER")->getSignedInt32(), houseNumber);
    assertEqual(testRun, pAddressResult->getField("STREET")->getString(), street);
    assertEqual(testRun, pAddressResult->getField("CITY")->getString(), city);
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
