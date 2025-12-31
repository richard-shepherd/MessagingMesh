using Newtonsoft.Json;
using System;
using System.IO;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// parsed version of config.json.
    /// </summary>
    internal class ParsedConfig
    {
        #region Config properties

        /// <summary>
        /// Gets or sets the gateway to connect to.
        /// </summary>
        public string Gateway { get; set; } = "";

        #endregion

        #region Enriched properties

        /// <summary>
        /// Gets the gateway hostname.
        /// </summary>
        public string GatewayHostname { get; private set; } = "";

        /// <summary>
        /// Gets the gateway port.
        /// </summary>
        public int GatewayPort { get; private set; } = 0;

        #endregion

        #region Public methods

        /// <summary>
        /// Reads JSON config from the file specified and parses it into the properties of this class.
        /// </summary>
        public static ParsedConfig fromFile(string filename)
        {
            var json = File.ReadAllText(filename);
            var config = JsonConvert.DeserializeObject<ParsedConfig>(json);
            config.enrichConfig();
            return config;
        }

        #endregion

        #region Private functions

        /// <summary>
        /// Post processing of the raw config.
        /// </summary>
        private void enrichConfig()
        {
            // We find the gateway hostname and port...
            var tokens = Gateway.Split(new char[] { ':' });
            GatewayHostname = tokens[0];
            GatewayPort = Convert.ToInt32(tokens[1]);
        }

        #endregion
    }
}
