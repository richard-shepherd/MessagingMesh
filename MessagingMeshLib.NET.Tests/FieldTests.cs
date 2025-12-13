using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MessagingMeshLib.NET.Tests
{
    /// <summary>
    /// Tests of the Field class.
    /// </summary>
    [TestClass]
    public class FieldTests
    {
        #region Test methods

        /// <summary>
        /// Tests tryGet for a string.
        /// </summary>
        [TestMethod]
        public void tryGet_String()
        {
            var field = new Field();

            // String...
            field.setString("London");
            var success = field.tryGetString(out var value);
            Assert.AreEqual(true, success);
            Assert.AreEqual("London", value);
        }

        /// <summary>
        /// Tests tryGet reading an integer field as a string.
        /// </summary>
        [TestMethod]
        public void tryGet_IntegerAsString()
        {
            var field = new Field();

            // String...
            field.setSignedInt32(123);
            var success = field.tryGetString(out var value);
            Assert.AreEqual(false, success);
        }

        #endregion
    }
}
