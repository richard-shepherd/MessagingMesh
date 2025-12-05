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
        /// Adds an item to the queue.
        /// </summary>
        public void add(ItemType item)
        {
            lock (m_queueLocker)
            {
                m_queue.Enqueue(item);
            }
            m_autoResetEvent.Set();
        }

        /// <summary>
        /// Waits for data to be available and gets the current contents of the queue, and resets the data being held.
        /// </summary>
        public Queue<ItemType> waitAndGetItems(int millisecondsTimeout)
        {
            m_autoResetEvent.WaitOne(millisecondsTimeout);
            lock (m_queueLocker)
            {
                if(m_queue.Count == 0)
                {
                    return null;
                }
                else
                {
                    var queue = m_queue;
                    m_queue = new();
                    return queue;
                }
            }
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

        // Queue of items and a locker for it...
        private Queue<ItemType> m_queue = new();
        private object m_queueLocker = new();

        // Signals when new data is available...
        private readonly AutoResetEvent m_autoResetEvent = new(false);

        #endregion
    }
}
