using System;
using System.Collections.Generic;

namespace MMListen
{
    /// <summary>
    /// Holds a parsed version of the command line.
    /// </summary>
    internal class CommandLine
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public CommandLine(string[] args)
        {
            // We add defaults...
            m_parsedArgs["hostname"] = "localhost";
            m_parsedArgs["port"] = "5050";
            m_parsedArgs["compact"] = "false";

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

                // If we have a value, we add it to the dictionary of parsed args...
                if(tokens.Length == 2)
                {
                    m_parsedArgs[key] = tokens[1];
                }
            }
        }

        /// <summary>
        /// Returns true if the parsed args includes the key specified.
        /// </summary>
        public bool hasKey(string key)
        {
            return m_parsedArgs.ContainsKey(key);
        }

        /// <summary>
        /// Returns the value for the key specified, converted to type T.
        /// Throws an exception if the key was not specified.
        /// </summary>
        public T get<T>(string key)
        {
            if(m_parsedArgs.TryGetValue(key, out var value))
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
