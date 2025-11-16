using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MessagingMeshLib.NET.Tests
{
    /// <summary>
    /// Tests of the Buffer class.
    /// </summary>
    [TestClass]
    public class BufferTests
    {
        /// <summary>
        /// Tests serializing a double.
        /// </summary>
        [TestMethod]
        public void serialize_double()
        {
            // We serialize a double...
            var buffer = new Buffer();
            buffer.write_double(3.14);

            // We check the buffer size...
            Assert.AreEqual(12, buffer.getBufferSize());

            // We check the bytes in the buffer...
            var data = buffer.getBuffer();

            // Size...
            Assert.AreEqual(0x0c, data[0]);
            Assert.AreEqual(0x00, data[1]);
            Assert.AreEqual(0x00, data[2]);
            Assert.AreEqual(0x00, data[3]);

            // Serialized double...
            Assert.AreEqual(0x1f, data[4]);
            Assert.AreEqual(0x85, data[5]);
            Assert.AreEqual(0xeb, data[6]);
            Assert.AreEqual(0x51, data[7]);
            Assert.AreEqual(0xb8, data[8]);
            Assert.AreEqual(0x1e, data[9]);
            Assert.AreEqual(0x09, data[10]);
            Assert.AreEqual(0x40, data[11]);
        }

        /// <summary>
        /// Tests serializing a string.
        /// </summary>
        [TestMethod]
        public void serialize_string()
        {
            // We serialize a string...
            var buffer = new Buffer();
            buffer.write_string("hello");

            // We check the buffer size...
            Assert.AreEqual(13, buffer.getBufferSize());

            // We check the bytes in the buffer...
            var data = buffer.getBuffer();

            // Size...
            Assert.AreEqual(0x0d, data[0]);
            Assert.AreEqual(0x00, data[1]);
            Assert.AreEqual(0x00, data[2]);
            Assert.AreEqual(0x00, data[3]);

            // String size...
            Assert.AreEqual(0x05, data[4]);
            Assert.AreEqual(0x00, data[5]);
            Assert.AreEqual(0x00, data[6]);
            Assert.AreEqual(0x00, data[7]);

            // String bytes...
            Assert.AreEqual((byte)'h', data[8]);
            Assert.AreEqual((byte)'e', data[9]);
            Assert.AreEqual((byte)'l', data[10]);
            Assert.AreEqual((byte)'l', data[11]);
            Assert.AreEqual((byte)'o', data[12]);
        }

        /// <summary>
        /// Tests serializing and deserializing a string.
        /// </summary>
        [TestMethod]
        public void serialize_and_deserialize_string()
        {
            // We serialize a string...
            var s1 = "hello";
            var buffer = new Buffer();
            buffer.write_string(s1);

            // We read the string from the buffer...
            buffer.resetPosition();
            var s2 = buffer.read_string();

            // We check we got back what we started with...
            Assert.AreEqual(s1, s2);
        }
    }
}
