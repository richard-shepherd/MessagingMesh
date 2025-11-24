using System;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Holds one field in a message.
    /// </summary>
    public class Field
    {
        #region Properties

        /// <summary>
        /// Gets the (approximate) size of the field when serialized.
        /// </summary>
        public int Size => getSize();

        #endregion

        #region Public types

        /// <summary>
        /// Types of data which can be held in a field.
        /// </summary>
        public enum DataType
        {
            NOT_SET,
            STRING,
            SIGNED_INT32,
            UNSIGNED_INT32,
            SIGNED_INT64,
            UNSIGNED_INT64,
            DOUBLE,
            MESSAGE,
            BOOL,
            BLOB
        };

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Field()
        {
        }

        /// <summary>
        /// Gets the field's name.
        /// </summary>
        public string getName()
        {
            return m_name;
        }

        /// <summary>
        /// Sets the field's name.
        /// </summary>
        public void setName(string name)
        {
            m_name = name;
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
        /// Gets the signed int32 held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public int getSignedInt32()
        {
            checkDataType(DataType.SIGNED_INT32);
            return (int)m_data;
        }

        /// <summary>
        /// Sets the field to hold a signed int32.
        /// </summary>
        public void setSignedInt32(int value)
        {
            m_dataType = DataType.SIGNED_INT32;
            m_data = value;
        }

        /// <summary>
        /// Gets the unsigned int32 held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public uint getUnsignedInt32()
        {
            checkDataType(DataType.UNSIGNED_INT32);
            return (uint)m_data;
        }

        /// <summary>
        /// Sets the field to hold an unsigned int32.
        /// </summary>
        public void setUnsignedInt32(uint value)
        {
            m_dataType = DataType.UNSIGNED_INT32;
            m_data = value;
        }

        /// <summary>
        /// Gets the signed int64 held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public long getSignedInt64()
        {
            checkDataType(DataType.SIGNED_INT64);
            return (long)m_data;
        }

        /// <summary>
        /// Sets the field to hold a signed int64.
        /// </summary>
        public void setSignedInt64(long value)
        {
            m_dataType = DataType.SIGNED_INT64;
            m_data = value;
        }

        /// <summary>
        /// Gets the unsigned int64 held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public ulong getUnsignedInt64()
        {
            checkDataType(DataType.UNSIGNED_INT64);
            return (ulong)m_data;
        }

        /// <summary>
        /// Sets the field to hold an unsigned int64.
        /// </summary>
        public void setUnsignedInt64(ulong value)
        {
            m_dataType = DataType.UNSIGNED_INT64;
            m_data = value;
        }

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

        /// <summary>
        /// Gets the bool held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public bool getBool()
        {
            checkDataType(DataType.BOOL);
            return (bool)m_data;
        }

        /// <summary>
        /// Sets the field to hold a bool.
        /// </summary>
        public void setBool(bool value)
        {
            m_dataType = DataType.BOOL;
            m_data = value;
        }

        /// <summary>
        /// Gets the BLOB held by the field.
        /// Throws a MessagingMeshException if the field does not hold this type.
        /// </summary>
        public byte[] getBLOB()
        {
            checkDataType(DataType.BLOB);
            return (byte[])m_data;
        }

        /// <summary>
        /// Sets the field to hold a BLOB.
        /// </summary>
        public void setBLOB(byte[] value)
        {
            m_dataType = DataType.BLOB;
            m_data = value;
        }

        #endregion

        #region Serialization

        /// <summary>
        /// Serializes the field to the current position of the buffer.
        /// </summary>
        internal void serialize(Buffer buffer)
        {
            // We serialize the field name...
            buffer.write_string(m_name);

            // We serialize the data type...
            buffer.write_byte((byte)m_dataType);

            // We serialize the data, depending on the type...
            switch (m_dataType)
            {
                case DataType.STRING:
                    buffer.write_string((string)m_data);
                    break;

                case DataType.SIGNED_INT32:
                    buffer.write_int32((int)m_data);
                    break;

                case DataType.UNSIGNED_INT32:
                    buffer.write_uint32((uint)m_data);
                    break;

                case DataType.SIGNED_INT64:
                    buffer.write_int64((long)m_data);
                    break;

                case DataType.UNSIGNED_INT64:
                    buffer.write_uint64((ulong)m_data);
                    break;

                case DataType.DOUBLE:
                    buffer.write_double((double)m_data);
                    break;

                case DataType.MESSAGE:
                    buffer.write_message((Message)m_data);
                    break;

                case DataType.BOOL:
                    buffer.write_bool((bool)m_data);
                    break;

                case DataType.BLOB:
                    buffer.write_blob((byte[])m_data);
                    break;

                default:
                    throw new MessagingMeshException($"Field.serialize() data-type {m_dataType} not handled");
            }
        }

        /// <summary>
        /// Deserializes the field from the current position in the buffer.
        /// </summary>
        internal void deserialize(Buffer buffer)
        {
            // We deserialize the name...
            m_name = buffer.read_string();

            // We deserialize the data type...
            m_dataType = (DataType)buffer.read_byte();

            // We deserialize the data, depending on the type...
            switch (m_dataType)
            {
                case DataType.STRING:
                    m_data = buffer.read_string();
                    break;

                case DataType.SIGNED_INT32:
                    m_data = buffer.read_int32();
                    break;

                case DataType.UNSIGNED_INT32:
                    m_data = buffer.read_uint32();
                    break;

                case DataType.SIGNED_INT64:
                    m_data = buffer.read_int64();
                    break;

                case DataType.UNSIGNED_INT64:
                    m_data = buffer.read_uint64();
                    break;

                case DataType.DOUBLE:
                    m_data = buffer.read_double();
                    break;

                case DataType.MESSAGE:
                    m_data = buffer.read_message();
                    break;

                case DataType.BOOL:
                    m_data = buffer.read_bool();
                    break;

                case DataType.BLOB:
                    m_data = buffer.read_blob();
                    break;

                default:
                    throw new MessagingMeshException($"Field.deserialize() data-type {m_dataType} not handled");
            }
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Checks that the data being held is the type specified.
        /// Throws a MessagingMeshException if it is not.
        /// </summary>
        private void checkDataType(DataType dataType)
        {
            if (m_dataType != dataType)
            {
                throw new MessagingMeshException($"Field {m_name} is not a {dataType}");
            }
        }

        /// <summary>
        /// Returns the (approximate) size of the field when serialized.
        /// </summary>
        private int getSize()
        {
            return getSize_NameAndType() + getSize_Field();
        }

        /// <summary>
        /// Returns the (approximate) size of the type and name when serialized.
        /// </summary>
        private int getSize_NameAndType()
        {
            return sizeof(int) + m_name.Length + sizeof(byte);
        }

        /// <summary>
        /// Returns the (approximate) size of the field when serialized.
        /// </summary>
        private int getSize_Field()
        {
            switch (m_dataType)
            {
                case DataType.STRING:
                    return sizeof(int) + ((string)m_data).Length;

                case DataType.SIGNED_INT32:
                    return sizeof(int);

                case DataType.UNSIGNED_INT32:
                    return sizeof(uint);

                case DataType.SIGNED_INT64:
                    return sizeof(long);

                case DataType.UNSIGNED_INT64:
                    return sizeof(ulong);

                case DataType.DOUBLE:
                    return sizeof(double);

                case DataType.MESSAGE:
                    return ((Message)m_data).Size; ;

                case DataType.BOOL:
                    return sizeof(bool);

                case DataType.BLOB:
                    return sizeof(int) + ((byte[])m_data).Length;

                default:
                    throw new MessagingMeshException($"Field.getSize() data-type {m_dataType} not handled");
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
