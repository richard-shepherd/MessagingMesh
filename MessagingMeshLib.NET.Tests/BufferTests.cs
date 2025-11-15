using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MessagingMeshLib.NET.Tests
{
    /// <summary>
    /// Tests of the Buffer.
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
    }
}
