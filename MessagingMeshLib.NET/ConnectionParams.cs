using System;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Parameters requesting a connection to the messaging mesh.
    /// Passed to the constructor of the Connection class.
    /// </summary>
    public class ConnectionParams
    {
        #region Public types

        /// <summary>
        /// Enum for how messages are dispatched to clients.
        /// </summary>
        public enum MessageDispatchEnum
        {
            /// <summary>
            /// Subscription callbacks are called on the Connection's messaging thread.
            /// </summary>
            CALLBACK_ON_MESSAGING_MESH_THREAD,

            /// <summary>
            /// Subscription callbacks are manually dispatched on the client thread by calling Connection.processMessageQueue().
            /// </summary>
            PROCESS_MESSAGE_QUEUE
        }

        #endregion

        #region Properties

        /// <summary>
        /// Gets or sets the gateway hostname or IP address.
        /// </summary>
        public string GatewayHost { get; set; } = "";

        /// <summary>
        /// Gets or sets the gateway port.
        /// </summary>
        public int GatewayPort { get; set; } = 0;

        /// <summary>
        /// Gets or sets the messaging-mesh service to join.
        /// </summary>
        public string Service { get; set; } = "";

        /// <summary>
        /// Gets or sets an ID identifying the client.
        /// Defaults to the exexcutable name + command line if not set.
        /// </summary>
        public string ClientID
        {
            get { return getClientID(); }
            set { m_clientID = value; }
        }

        /// <summary>
        /// Gets or sets how subscribed messages are dispatched to client code.
        /// </summary>
        public MessageDispatchEnum MessageDispatch { get; set; } = MessageDispatchEnum.PROCESS_MESSAGE_QUEUE;

        #endregion

        #region Private functions

        /// <summary>
        /// Returns the client iD, creating a default one if it has not been specified.
        /// </summary>
        private string getClientID()
        {
            if(!String.IsNullOrEmpty(m_clientID))
            {
                return m_clientID;
            }
            else
            {
                return getDefaultClientID();
            }
        }

        /// <summary>
        /// Returns a default client ID made up of the executable name and command-line parameters.
        /// </summary>
        private static string getDefaultClientID()
        {
            try
            {
                using var currentProcess = Process.GetCurrentProcess();
                var executableName = Path.GetFileNameWithoutExtension(currentProcess.ProcessName);

                // Get command-line args (excluding the executable path itself)
                var args = Environment.GetCommandLineArgs().Skip(1);
                var commandLineParams = string.Join(" ", args);

                return string.IsNullOrEmpty(commandLineParams)? executableName : $"{executableName} {commandLineParams}";
            }
            catch
            {
                return "[unknown-client-ID]";
            }
        }

        #endregion

        #region Private data

        // The client ID...
        private string m_clientID = "";

        #endregion
    }
}
