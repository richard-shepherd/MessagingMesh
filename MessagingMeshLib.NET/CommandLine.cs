using System;
using System.Collections.Generic;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Holds a parsed version of the command line.
    /// </summary>
    public class CommandLine
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public CommandLine(string[] args)
        {
            // We parse the command line...
            foreach (var arg in args)
            {
                // Command line arguments look like -key=value
                var tokens = arg.Split(new char[] { '=' });
                var key = tokens[0];
                key = key.TrimStart('-');
                key = key.ToLower();

                // We check for variations of help...
                if(key == "help" || key == "h" || key == "?")
                {
                    m_parsedArgs["help"] = "true";
                }

                if(tokens.Length == 2)
                {
                    // We have an argument like key=value...
                    m_parsedArgs[key] = tokens[1];
                }
                else
                {
                    // We have a key with no value...
                    m_parsedArgs[key] = "";
                }
            }
        }

        /// <summary>
        /// Adds a default to be used if the key is not present on the command-line.
        /// The key should be specified without leading dashes, eg "hostname" instead of "-hostname".
        /// </summary>
        public void addDefault(string key, string value)
        {
            key = key.ToLower();
            if (!hasKey(key))
            {
                m_parsedArgs[key] = value;
            }
        }

        /// <summary>
        /// Returns true if the parsed args includes the key specified.
        /// The key should be specified without leading dashes, eg "hostname" instead of "-hostname".
        /// </summary>
        public bool hasKey(string key)
        {
            key = key.ToLower();
            return m_parsedArgs.ContainsKey(key);
        }

        /// <summary>
        /// Returns the value for the key specified, converted to type T.
        /// The key should be specified without leading dashes, eg "hostname" instead of "-hostname".
        /// Throws an exception if the key was not specified.
        /// </summary>
        public T get<T>(string key)
        {
            key = key.ToLower();
            if (m_parsedArgs.TryGetValue(key, out var value))
            {
                return (T)Convert.ChangeType(value, typeof(T));
            }
            throw new Exception($"Key '{key}' not found in parsed arguments");
        }

        #endregion

        #region Private data

        // Command line parsed from -key=value...
        private Dictionary<string, string> m_parsedArgs = new();

        #endregion
    }
}
