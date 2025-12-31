using System;
using System.Collections.Generic;
using System.Threading;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Not so much a logger as a log-broker.
    /// 
    /// Log messages can be logged to the Logger and clients can register
    /// for callbacks when messages are logged. The client can then show
    /// or log the messages.
    /// </summary>
    public class Logger
    {
        #region Public types

        /// <summary>
        /// Log level enum.
        /// </summary>
        public enum LogLevel
        {
            DEBUG,
            INFO,
            WARN,
            ERROR,
            FATAL
        };

        /// <summary>
        /// Function signature for log callbacks.
        /// </summary>
        public delegate void Callback(LogLevel logLevel, string message);

        #endregion

        #region Public methods

        /// <summary>
        /// Registers a function to be called when messages are logged.
        /// </summary>
        public static void registerCallback(Callback callback)
        {
            lock (m_callbacksLocker)
            {
                m_callbacks.Add(callback);
            }
        }

        /// <summary>
        /// Logs a message at the level specified.
        /// </summary>
        public static void log(LogLevel logLevel, string message)
        {
            // We take a copy of the callbacks (to avoid locking over the callbacks themselves)...
            List<Callback> callbacks;
            lock (m_callbacksLocker)
            {
                callbacks = new List<Callback>(m_callbacks);
            }

            // We convert a null message to text...
            message ??= "(null)";

            // We add info the the message...
            var threadName = Thread.CurrentThread.Name;
            var messageToLog = $"MessagingMesh ({threadName}): {message}";

            // We notify the registered callbacks...
            foreach (var callback in callbacks)
            {
                try
                {
                    callback(logLevel, messageToLog);
                }
                catch (Exception)
                {
                    // We do not do anything here. We do not log, as this is the logger!
                }
            }
        }

        /// <summary>
        /// Logs an exception including the stack trace.
        /// </summary>
        public static void log(Exception ex)
        {
            log(LogLevel.ERROR, ex.ToString());
        }

        /// <summary>
        /// Logs a message at DEBUG level.
        /// </summary>
        public static void debug(string message)
        {
            log(LogLevel.DEBUG, message);
        }

        /// <summary>
        /// Logs a message at INFO level.
        /// </summary>
        public static void info(string message)
        {
            log(LogLevel.INFO, message);
        }

        /// <summary>
        /// Logs a message at WARN level.
        /// </summary>
        public static void warn(string message)
        {
            log(LogLevel.WARN, message);
        }

        /// <summary>
        /// Logs a message at ERROR level.
        /// </summary>
        public static void error(string message)
        {
            log(LogLevel.ERROR, message);
        }

        /// <summary>
        /// Logs a message at FATAL level.
        /// </summary>
        public static void fatal(string message)
        {
            log(LogLevel.FATAL, message);
        }

        #endregion

        #region Private data

        // List of registered callbacks and an object to lock it...
        private static List<Callback> m_callbacks = new();
        private static object m_callbacksLocker = new();

        #endregion
    }
}
