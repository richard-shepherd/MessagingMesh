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
            SIGNED_INT,
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
        /// Constructor.
        /// </summary>
        public Field()
        {
        }

        #endregion

        #region Getters and setters for field types

        /// <summary>
        /// Gets the string held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public string getString()
        {
            checkDataType(DataType.STRING);
            return (string)m_data;
        }

        /// <summary>
        /// Sets the field to hold a string.
        /// </summary>
        public void setString(string value)
        {
            m_dataType = DataType.STRING;
            m_data = value;
        }

        /// <summary>
        /// Gets the signed int held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public int getSignedInt()
        {
            checkDataType(DataType.SIGNED_INT);
            return (int)m_data;
        }

        /// <summary>
        /// Sets the field to hold a signed int.
        /// </summary>
        public void setSignedInt(int value)
        {
            m_dataType = DataType.SIGNED_INT;
            m_data = value;
        }

        //// Gets the double held by the field.
        //// Throws a MessagingMesh::Exception  if the field does not hold this type.

        /// <summary>
        /// Gets the double held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public double getDouble()
        {
            checkDataType(DataType.DOUBLE);
            return (double)m_data;
        }

        /// <summary>
        /// Sets the field to hold a double.
        /// </summary>
        public void setDouble(double value)
        {
            m_dataType = DataType.DOUBLE;
            m_data = value;
        }

        /// <summary>
        /// Gets the Message held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public Message getMessage()
        {
            checkDataType(DataType.MESSAGE);
            return (Message)m_data;
        }

        /// <summary>
        /// Sets the field to hold a message.
        /// </summary>
        public void setMessage(Message value)
        {
            m_dataType = DataType.MESSAGE;
            m_data = value;
        }

        #endregion

        #region Serialization and deserialization

        /// <summary>
        /// Serializes the field to the current position of the buffer.
        /// </summary>
        internal void serialize(Buffer buffer)
        {
            // RSSTODO: WRITE THIS!!!
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deserializes the field from the current position in the buffer.
        /// </summary>
        internal void deserialize(Buffer buffer)
        {
            // RSSTODO: WRITE THIS!!!
            throw new NotImplementedException();
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Checks that the data being held is the type specified.
        /// Throws a MessagingMeshException if it is not.
        /// </summary>
        private void checkDataType(DataType dataType)
        {
            if (m_dataType != DataType.STRING)
            {
                throw new MessagingMeshException($"Field {m_name} is not a {dataType}");
            }
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
