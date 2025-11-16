using System.Collections.Generic;
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
    internal class ThreadsafeConsumableList<ItemType> where ItemType : class
    {
        #region Public methods

        /// <summary>
        /// Constructor.
        /// </summary>
        public ThreadsafeConsumableList()
        {
        }

        /// <summary>
        /// Adds an item to the list.
        /// </summary>
        public void add(ItemType item)
        {
            lock(m_locker)
            {
                m_items.Add(item);
                m_autoResetEvent.Set();
            }
        }

        /// <summary>
        /// Gets the current contents of the list, and clears the data being held.
        /// </summary>
        public List<ItemType> getItems()
        {
            lock (m_locker)
            {
                var result = m_items;
                m_items = new();
                return result;
            }
        }

        /// <summary>
        /// Waits for data to be available and gets the current contents of the vector, 
        /// and clears the data being held.
        /// </summary>
        public List<ItemType> waitAndGetItems(int millisecondsTimeout)
        {
            m_autoResetEvent.WaitOne(millisecondsTimeout);
            lock (m_locker)
            {
                var result = m_items;
                m_items = new();
                return result;
            }
        }

        #endregion

        #region Private data

        // List of items and a locker for them...
        private List<ItemType> m_items = new();
        private readonly object m_locker = new();

        // Signals when new data is available...
        private readonly AutoResetEvent m_autoResetEvent = new(false);

        #endregion
    }
}
