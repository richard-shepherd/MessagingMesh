using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Xml.Linq;

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

        #endregion

        #region Field add and get methods

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
                if (!m_mapNameToField.ContainsKey(name))
                {
                    m_mapNameToField.Add(name, field);
                }
            }
        }

        #endregion

        #region Private functions

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
            if (!m_mapNameToField.ContainsKey(name))
            {
                m_mapNameToField.Add(name, field);
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
