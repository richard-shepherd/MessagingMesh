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
            // RSSTODO: WRITE THIS!!!
            throw new NotImplementedException();
        }

        /// <summary>
        /// Deserializes the field from the current position in the buffer.
        /// </summary>
        void deserialize(Buffer buffer)
        {
            // RSSTODO: WRITE THIS!!!
            throw new NotImplementedException();
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
            return m_data as string;
        }

        /// <summary>
        /// Sets the field to hold a string.
        /// </summary>
        void setString(string value)
        {
            m_dataType = DataType.STRING;
            m_data = value;
        }

        //// Gets the signed int32 held by the field.
        //// Throws a MessagingMesh::Exception if the field does not hold this type.
        //int32_t getSignedInt32() const;

        //// Sets the field to hold a signed int32.
        //void setSignedInt32(int32_t value);

        //// Gets the double held by the field.
        //// Throws a MessagingMesh::Exception  if the field does not hold this type.
        //double getDouble() const;

        //// Sets the field to hold a double.
        //void setDouble(double value);

        //// Gets the message held by the field.
        //// Throws a MessagingMesh::Exception if the field does not hold this type.
        //const ConstMessagePtr& getMessage() const;

        //// Sets the field to hold a message.
        //void setMessage(const ConstMessagePtr& value);

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
