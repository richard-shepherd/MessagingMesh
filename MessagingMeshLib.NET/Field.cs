using System;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Holds one field in a message.
    /// </summary>
    public class Field
    {
        #region Public types

        /// <summary>
        /// Types of data which can be held in a field.
        /// </summary>
        public enum DataType
        {
            NOT_SET,
            STRING,
            SIGNED_INT32,
            DOUBLE,
            MESSAGE
        };

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets the field's name.
        /// </summary>
        public string Name
        {
            get { return m_name; } 
            set { m_name = value; }
        }

        #endregion

        #region Public methods

        /// <summary>
        /// Serializes the field to the current position of the buffer.
        /// </summary>
        void serialize(Buffer buffer)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deserializes the field from the current position in the buffer.
        /// </summary>
        void deserialize(Buffer buffer)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region Private data

        // The field's name...
        private string m_name;

        // The field's type...
        private DataType m_dataType;

        // The field's data as an object...
        private object m_data;

        #endregion
    }
}
