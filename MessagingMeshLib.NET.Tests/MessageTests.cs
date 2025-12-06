using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

namespace MessagingMeshLib.NET.Tests
{
    /// <summary>
    /// Tests of the Message class.
    /// </summary>
    [TestClass]
    public class MessageTests
    {
        #region Test methods

        /// <summary>
        /// Tests serializing a message holding all supported field types.
        /// </summary>
        [TestMethod]
        public void messageSerialization()
        {
            string s = "hello, world!";
            int i32_1 = 123456;
            int i32_2 = -123456;
            uint ui32 = 3123456789;
            long i64_1 = 4123456789123456789;
            long i64_2 = -4123456789123456789;
            ulong ui64 = 11123456789123456789;
            double d = 123.456;
            bool b1 = true;
            bool b2 = false;
            byte[] blob = {1, 2, 3, 230, 240, 250 };

            // We create a message holding all the types...
            var m = new Message();
            m.addString("s", s);
            m.addSignedInt32("i32_1", i32_1);
            m.addSignedInt32("i32_2", i32_2);
            m.addUnsignedInt32("ui32", ui32);
            m.addSignedInt64("i64_1", i64_1);
            m.addSignedInt64("i64_2", i64_2);
            m.addUnsignedInt64("ui64", ui64);
            m.addDouble("d", d);
            m.addBool("b1", b1);
            m.addBool("b2", b2);
            m.addBLOB("blob", blob);

            // We create a sub-message and add it...
            var sm = new Message();
            sm.addString("s", s);
            sm.addDouble("d", d);
            m.addMessage("sm", sm);

            // We serialize the message to a buffer...
            var buffer = new Buffer();
            m.serialize(buffer);

            // We deserialize to a new message...
            buffer.resetPosition();
            var m2 = new Message();
            m2.deserialize(buffer);

            // We check the values...
            testMessageFields(m2);

            // We save the buffer for use with deserialization tests...
            saveBuffer("..\\..\\..\\TestData\\SerializedBuffer-cs.bin", buffer);
        }

        /// <summary>
        /// Tests deserializing a message from C++ data.
        /// </summary>
        [TestMethod]
        public void messageDeserialize_CPP()
        {
            var buffer = loadBuffer("..\\..\\..\\TestData\\SerializedBuffer-cpp.bin");
            var message = new Message();
            message.deserialize(buffer);
            testMessageFields(message);
        }

        /// <summary>
        /// Tests deserializing a message from C# data.
        /// </summary>
        [TestMethod]
        public void messageDeserialize_CS()
        {
            var buffer = loadBuffer("..\\..\\..\\TestData\\SerializedBuffer-cs.bin");
            var message = new Message();
            message.deserialize(buffer);
            testMessageFields(message);
        }

        /// <summary>
        /// Tests updating a field.
        /// </summary>
        [TestMethod]
        public void updateField_SameType()
        {
            // We add a field to a message...
            var message = new Message();
            message.addSignedInt32("A", 123);

            // We get the field and update the value...
            var field = message.getField("A");
            field.setSignedInt32(234);

            // We check the value...
            Assert.AreEqual(234, message.getSignedInt32("A"));
        }

        /// <summary>
        /// Tests updating a field.
        /// </summary>
        [TestMethod]
        public void updateField_DifferentType()
        {
            // We add a field to a message...
            var message = new Message();
            message.addSignedInt32("A", 123);

            // We get the field and update the value...
            var field = message.getField("A");
            field.setDouble(234.567);

            // We check the value...
            Assert.AreEqual(234.567, message.getDouble("A"));
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Tests the the message has the expected fields.
        /// </summary>
        private void testMessageFields(Message m)
        {
            string s = "hello, world!";
            int i32_1 = 123456;
            int i32_2 = -123456;
            uint ui32 = 3123456789;
            long i64_1 = 4123456789123456789;
            long i64_2 = -4123456789123456789;
            ulong ui64 = 11123456789123456789;
            double d = 123.456;
            bool b1 = true;
            bool b2 = false;
            byte[] blob = { 1, 2, 3, 230, 240, 250 };

            // We check the values...
            Assert.AreEqual(s, m.getString("s"));
            Assert.AreEqual(i32_1, m.getSignedInt32("i32_1"));
            Assert.AreEqual(i32_2, m.getSignedInt32("i32_2"));
            Assert.AreEqual(ui32, m.getUnsignedInt32("ui32"));
            Assert.AreEqual(i64_1, m.getSignedInt64("i64_1"));
            Assert.AreEqual(i64_2, m.getSignedInt64("i64_2"));
            Assert.AreEqual(ui64, m.getUnsignedInt64("ui64"));
            Assert.AreEqual(d, m.getDouble("d"));
            Assert.AreEqual(b1, m.getBool("b1"));
            Assert.AreEqual(b2, m.getBool("b2"));

            // The BLOB...
            var blob2 = m.getBLOB("blob");
            Assert.HasCount(6, blob2);
            Assert.AreEqual(blob[0], blob2[0]);
            Assert.AreEqual(blob[1], blob2[1]);
            Assert.AreEqual(blob[2], blob2[2]);
            Assert.AreEqual(blob[3], blob2[3]);
            Assert.AreEqual(blob[4], blob2[4]);
            Assert.AreEqual(blob[5], blob2[5]);

            // The sub-message...
            var sm = m.getMessage("sm");
            Assert.AreEqual(s, sm.getString("s"));
            Assert.AreEqual(d, sm.getDouble("d"));
        }

        /// <summary>
        /// Saves the buffer for use with deserialization tests.
        /// </summary>
        private void saveBuffer(string filename, Buffer buffer)
        {
            using (var fs = new FileStream(filename, FileMode.Create))
            {
                fs.Write(buffer.getBuffer(), 0, buffer.getBufferSize());
            }
        }

        /// <summary>
        /// Loads a buffer from the file.
        /// </summary>
        private Buffer loadBuffer(string filename)
        {
            var data = File.ReadAllBytes(filename);
            var buffer = new Buffer();
            buffer.readNetworkMessage(data, data.Length, 0);
            buffer.resetPosition();
            return buffer;
        }

        #endregion
    }
}
