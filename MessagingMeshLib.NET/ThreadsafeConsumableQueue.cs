using System.Collections.Concurrent;
using System.Threading;

namespace MessagingMeshLib.NET
{
    /// <summary>
    /// Allows you to add data from producer threads and retrieve data
    /// from a consumer thread.
    /// 
    /// When you retrieve data you are given a copy of all the data currently 
    /// available, and the internal data is cleared.
    /// </summary>
    internal class ThreadsafeConsumableQueue<ItemType> where ItemType : class
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public ThreadsafeConsumableQueue()
        {
        }

        /// <summary>
        /// Adds an item to the list.
        /// </summary>
        public void add(ItemType item)
        {
            m_queue.Enqueue(item);
            m_autoResetEvent.Set();
        }

        /// <summary>
        /// Gets the current contents of the list, and clears the data being held.
        /// </summary>
        public ConcurrentQueue<ItemType> getItems()
        {
            var newQueue = new ConcurrentQueue<ItemType>();
            var oldQueue = Interlocked.Exchange(ref m_queue, newQueue);
            return oldQueue;
        }

        /// <summary>
        /// Waits for data to be available and gets the current contents of the vector, 
        /// and clears the data being held.
        /// </summary>
        public ConcurrentQueue<ItemType> waitAndGetItems(int millisecondsTimeout)
        {
            m_autoResetEvent.WaitOne(millisecondsTimeout);
            return getItems();
        }

        /// <summary>
        /// Sets the auto-reset event so that a waitAndGetItems() call will complete before the timeout has elapsed.
        /// </summary>
        public void wakeUp()
        {
            m_autoResetEvent.Set();
        }

        #endregion

        #region Private data

        private ConcurrentQueue<ItemType> m_queue = new();

        // Signals when new data is available...
        private readonly AutoResetEvent m_autoResetEvent = new(false);

        #endregion
    }
}
