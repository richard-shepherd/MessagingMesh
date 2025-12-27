using System;
using System.Collections.Generic;
using System.Text;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// A message which can be sent via the Messaging Mesh.
    /// Holds a list of named Fields which can be accessed by
    /// name or by index. You can add multiple fields with the
    /// same name.
    /// </summary>
    public class Message
    {
        #region Properties

        /// <summary>
        /// Gets the (approximate) size of the message when serialized.
        /// </summary>
        public int Size
        {
            get 
            {
                // The message size is: 
                // - sizeof(int): The number of fields
                // - Size of each field
                var size = sizeof(int);
                foreach (var field in m_fields)
                {
                    size += field.Size;
                }
                return size;
            }
        }

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Message()
        {
        }

        /// <summary>
        /// Gets a field by name.
        /// Throws a MessagingMeshException if the message does not hold the field requested.
        /// </summary>
        public Field getField(string name)
        {
            if(m_mapNameToField.TryGetValue(name, out var field))
            {
                return field;
            }
            throw new MessagingMeshException($"Field {name} not in message");
        }

        /// <summary>
        /// Returns a list of all fields in the message.
        /// NOTE: This is a copy of the list held by the message, so changes to the returned list
        ///       will not change the fields held by the message.
        /// </summary>
        public List<Field> getAllFields()
        {
            return new List<Field>(m_fields);
        }

        /// <summary>
        /// Returns a string rendering of the Message for example for use with the MMListen app.
        /// </summary>
        public string toMMListenString(string heading, int indent = 0)
        {
            var sb = new StringBuilder();
            var indent_field = new string(' ', indent * 2 + 2);

            // Heading...
            sb.AppendLine($"{heading}:");

            // Fields...
            for (var i = 0; i < m_fields.Count; ++i)
            {
                var field = m_fields[i];
                sb.Append($"{indent_field}{field.toMMListenString(indent)}");
                if (i < m_fields.Count - 1)
                {
                    sb.AppendLine("");
                }
            }

            return sb.ToString();
        }

        #endregion

        #region String

        /// <summary>
        /// Adds a string field to the message.
        /// </summary>
        public void addString(string name, string value)
        {
            addField(name, (field) => field.setString(value));
        }

        /// <summary>
        /// Gets a string from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public string getString(string name)
        {
            return getField(name).getString();
        }

        /// <summary>
        /// Tries to get a string from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetString(string name, out string value)
        {
            return tryGetValue(name, out value, (Field f, out string v) => f.tryGetString(out v));
        }

        #endregion

        #region SignedInt32

        /// <summary>
        /// Adds a signed int32 field to the message.
        /// </summary>
        public void addSignedInt32(string name, int value)
        {
            addField(name, (field) => field.setSignedInt32(value));
        }

        /// <summary>
        /// Gets a signed int32 from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public int getSignedInt32(string name)
        {
            return getField(name).getSignedInt32();
        }

        /// <summary>
        /// Tries to get a signed int32 from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetSignedInt32(string name, out int value)
        {
            return tryGetValue(name, out value, (Field f, out int v) => f.tryGetSignedInt32(out v));
        }

        #endregion

        #region UnsignedInt32

        /// <summary>
        /// Adds an unsigned int32 field to the message.
        /// </summary>
        public void addUnsignedInt32(string name, uint value)
        {
            addField(name, (field) => field.setUnsignedInt32(value));
        }

        /// <summary>
        /// Gets an unsigned int32 from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public uint getUnsignedInt32(string name)
        {
            return getField(name).getUnsignedInt32();
        }

        /// <summary>
        /// Tries to get an unsigned int32 from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetUnsignedInt32(string name, out uint value)
        {
            return tryGetValue(name, out value, (Field f, out uint v) => f.tryGetUnsignedInt32(out v));
        }

        #endregion

        #region SignedInt64

        /// <summary>
        /// Adds a signed int64 field to the message.
        /// </summary>
        public void addSignedInt64(string name, long value)
        {
            addField(name, (field) => field.setSignedInt64(value));
        }

        /// <summary>
        /// Gets a signed int64 from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public long getSignedInt64(string name)
        {
            return getField(name).getSignedInt64();
        }

        /// <summary>
        /// Tries to get a signed int64 from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetSignedInt64(string name, out long value)
        {
            return tryGetValue(name, out value, (Field f, out long v) => f.tryGetSignedInt64(out v));
        }

        #endregion

        #region UnsignedInt64

        /// <summary>
        /// Adds an unsigned int64 field to the message.
        /// </summary>
        public void addUnsignedInt64(string name, ulong value)
        {
            addField(name, (field) => field.setUnsignedInt64(value));
        }

        /// <summary>
        /// Gets an unsigned int64 from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public ulong getUnsignedInt64(string name)
        {
            return getField(name).getUnsignedInt64();
        }

        /// <summary>
        /// Tries to get an unsigned int64 from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetUnsignedInt64(string name, out ulong value)
        {
            return tryGetValue(name, out value, (Field f, out ulong v) => f.tryGetUnsignedInt64(out v));
        }

        #endregion

        #region Double

        /// <summary>
        /// Adds a double field to the message.
        /// </summary>
        public void addDouble(string name, double value)
        {
            addField(name, (field) => field.setDouble(value));
        }

        /// <summary>
        /// Gets a double from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public double getDouble(string name)
        {
            return getField(name).getDouble();
        }

        /// <summary>
        /// Tries to get a double from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetDouble(string name, out double value)
        {
            return tryGetValue(name, out value, (Field f, out double v) => f.tryGetDouble(out v));
        }

        #endregion

        #region Message

        /// <summary>
        /// Adds a sub-message field to the message.
        /// </summary>
        public void addMessage(string name, Message value)
        {
            addField(name, (field) => field.setMessage(value));
        }

        /// <summary>
        /// Gets a sub-message from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public Message getMessage(string name)
        {
            return getField(name).getMessage();
        }

        /// <summary>
        /// Tries to get a Message from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetMessage(string name, out Message value)
        {
            return tryGetValue(name, out value, (Field f, out Message v) => f.tryGetMessage(out v));
        }

        #endregion

        #region Bool

        /// <summary>
        /// Adds a bool field to the message.
        /// </summary>
        public void addBool(string name, bool value)
        {
            addField(name, (field) => field.setBool(value));
        }

        /// <summary>
        /// Gets a bool from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public bool getBool(string name)
        {
            return getField(name).getBool();
        }

        /// <summary>
        /// Tries to get a bool from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetBool(string name, out bool value)
        {
            return tryGetValue(name, out value, (Field f, out bool v) => f.tryGetBool(out v));
        }

        #endregion

        #region BLOB

        /// <summary>
        /// Adds a BLOB field to the message.
        /// </summary>
        public void addBLOB(string name, byte[] value)
        {
            addField(name, (field) => field.setBLOB(value));
        }

        /// <summary>
        /// Gets a BLOB from the named field.
        /// Throws a MessagingMeshException if the message does not hold the field requested
        /// or if the field is not the requested type.
        /// </summary>
        public byte[] getBLOB(string name)
        {
            return getField(name).getBLOB();
        }

        /// <summary>
        /// Tries to get a BLOB from the named field.
        /// Returns true if successful, false if the field does not exist or is not the correct type.
        /// </summary>
        public bool tryGetBLOB(string name, out byte[] value)
        {
            return tryGetValue(name, out value, (Field f, out byte[] v) => f.tryGetBLOB(out v));
        }

        #endregion

        #region Serialization

        /// <summary>
        /// Serializes the message to the current position in the buffer.
        /// </summary>
        internal void serialize(Buffer buffer)
        {
            // We write the number of fields...
            var fieldCount = m_fields.Count;
            buffer.write_int32(fieldCount);

            // We write each field...
            foreach (var field in m_fields)
            {
                buffer.write_field(field);
            }
        }

        // Deserializes the message from the current position in the buffer.
        internal void deserialize(Buffer buffer)
        {
            // We find the number of fields...
            var fieldCount = buffer.read_int32();

            // We read each field and add them to the message...
            for (var i = 0; i < fieldCount; ++i)
            {
                var field = buffer.read_field();
                m_fields.Add(field);
                var name = field.getName();
                if (!m_mapNameToField.TryGetValue(name, out var existingField))
                {
                    m_mapNameToField[name] = field;
                }
            }
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Delegate used by tryGetValue (below).
        /// </summary>
        private delegate bool TryGetDelegate<T>(Field field, out T value);

        /// <summary>
        /// Generic tryGet method for fields.
        /// </summary>
        private bool tryGetValue<T>(string name, out T value, TryGetDelegate<T> tryGet)
        {
            if (m_mapNameToField.TryGetValue(name, out var field))
            {
                return tryGet(field, out value);
            }
            value = default;
            return false;
        }

        /// <summary>
        /// Private addField helper used by the public add[type]() methods to create
        /// and set up a field and add it to the message.
        /// </summary>
        private void addField(string name, Action<Field> valueSetter)
        {
            // We create a field and set its name...
            var field = new Field();
            field.setName(name);

            // We call the function (lambda) to set the value...
            valueSetter(field);

            // We add the field to the list of fields...
            m_fields.Add(field);

            // We add the field to the map of name->first-field-for-name...
            if (!m_mapNameToField.TryGetValue(name, out var existingField))
            {
                m_mapNameToField[name] = field;
            }
        }

        #endregion

        #region Private data

        // List of fields in the message, in the order they were added...
        private readonly List<Field> m_fields = new();

        // Map of field name to the first field with that name...
        private readonly Dictionary<string, Field> m_mapNameToField = new();

        #endregion
    }
}
