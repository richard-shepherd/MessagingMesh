using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MessagingMeshLib.NET.Tests
{
    /// <summary>
    /// Tests of the Message class.
    /// </summary>
    [TestClass]
    public class MessageTests
    {
        /// <summary>
        /// Tests serializing a double.
        /// </summary>
        [TestMethod]
        public void messageSerialization()
        {
            string name = "Charles";
            double age = 47.5;
            bool hasDog = true;
            bool hasCat = false;
            int houseNumber = 3;
            string street = "London Road";
            string city = "Bristol";

            // We create a message...
            var person = new Message();
            person.addString("NAME", name);
            person.addDouble("AGE", age);
            person.addBool("HAS_DOG", hasDog);
            person.addBool("HAS_CAT", hasCat);

            // We add a sub-message...
            var address = new Message();
            address.addSignedInt("HOUSE-NUMBER", houseNumber);
            address.addString("STREET", street);
            address.addString("CITY", city);
            person.addMessage("ADDRESS", address);

            // We serialize the message...
            var buffer = new Buffer();
            person.serialize(buffer);

            // We deserialize the message...
            buffer.resetPosition();
            var result = new Message();
            result.deserialize(buffer);

            Assert.AreEqual(name, result.getString("NAME"));
            Assert.AreEqual(age, result.getDouble("AGE"));
            Assert.AreEqual(hasDog, result.getBool("HAS_DOG"));
            Assert.AreEqual(hasCat, result.getBool("HAS_CAT"));

            var addressResult = result.getMessage("ADDRESS");
            Assert.AreEqual(houseNumber, addressResult.getSignedInt("HOUSE-NUMBER"));
            Assert.AreEqual(street, addressResult.getString("STREET"));
            Assert.AreEqual(city, addressResult.getString("CITY"));
        }
    }
}
