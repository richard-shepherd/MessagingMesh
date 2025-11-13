using System;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Exception thrown by messaging mesh code.
    /// </summary>
    public class MessagingMeshException : Exception
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public MessagingMeshException(string message)
            : base(message)
        {
        }

        #endregion
    }
}
