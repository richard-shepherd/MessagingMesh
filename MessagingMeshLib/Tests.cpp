#include "Tests.h"
#include "Message.h"
#include "Field.h"
#include "Buffer.h"
using namespace MessagingMesh;

// Tests message serialization and deserialization.
void Tests::messageSerialization()
{
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

    assertEqual(pResult->getField("NAME")->getString(), name);
    assertEqual(pResult->getField("AGE")->getDouble(), age);

    auto pAddressResult = pResult->getField("ADDRESS")->getMessage();
    assertEqual(pAddressResult->getField("HOUSE-NUMBER")->getSignedInt32(), houseNumber);
    assertEqual(pAddressResult->getField("STREET")->getString(), street);
    assertEqual(pAddressResult->getField("CITY")->getString(), city);
}
