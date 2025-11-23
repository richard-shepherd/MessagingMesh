using System;
using static MessagingMeshLib.NET.Connection;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Manages the lifetime of a subscription.
    /// 
    /// Subscription objects are returned by Connection.subscribe(). The subscription
    /// will be removed when the object is disposed.
    /// </summary>
    public class Subscription : IDisposable
    {
        #region Public types

        /// <summary>
        /// Callback info.
        /// </summary>
        public class CallbackInfo
        {
            /// <summary>
            /// The callback.
            /// </summary>
            public SubscriptionCallback Callback { get; set; } = null;

            /// <summary>
            /// A tag / closure passed to the callback.
            /// </summary>
            public object Tag { get; set; } = null;
        }

        #endregion

        #region Properties

        /// <summary>
        /// Gets the subscription ID.
        /// </summary>
        public uint SubscriptionID => m_subscriptionID;

        #endregion

        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public Subscription(Connection connection, string subject, CallbackInfo callbackInfo, uint subscriptionID)
        {
            m_connection = connection;
            m_subject = subject;
            m_callbackInfo = callbackInfo;
            m_subscriptionID = subscriptionID;
        }

        #endregion

        #region IDisposable implementation

        public virtual void Dispose()
        {
            if (IsDisposed) return;

            // We release the subscription...
            m_connection.releaseSubscription(m_subject, m_callbackInfo);

            IsDisposed = true;
        }

        protected bool IsDisposed { get; private set; }

        #endregion

        #region Private data

        // Construction params...
        private readonly Connection m_connection;
        private readonly string m_subject;
        private readonly CallbackInfo m_callbackInfo;
        private readonly uint m_subscriptionID;

        #endregion
    }
}
