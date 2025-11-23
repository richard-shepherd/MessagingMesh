#pragma once
#include <vector>
#include <set>
#include <memory>
#include <mutex>
#include "AutoResetEvent.h"

namespace MessagingMesh
{
    /// <summary>
    /// Allows you to add data from producer threads and retrieve data
    /// from a consumer thread.
    /// 
    /// When you retrieve data you are given a copy of all the data currently 
    /// available, and the internal data is cleared.
    /// </summary>
    template<typename ItemType, typename UniqueKeyType = int>
    class ThreadsafeConsumableVector
    {
    // Public types...
    public:
        // Vector of ItemType.
        using VecItemType = std::vector<ItemType>;

        // Shared pointer for VecItemType.
        using VecItemTypePtr = std::shared_ptr<VecItemType>;

    // Public methods...
    public:
        // Constructor.
        ThreadsafeConsumableVector()
        {
            // We create the items vector...
            m_items = std::make_shared<VecItemType>();
        }

        // Adds an item to the vector.
        void add(ItemType& item)
        {
            std::scoped_lock lock(m_mutex);
            m_items->push_back(item);
            m_autoResetEvent.set();
        }

        // Adds an item to the vector if the key has not already been registered.
        // Returns true if the item was added, false if not.
        bool addUnique(const UniqueKeyType& key, ItemType& item)
        {
            std::scoped_lock lock(m_mutex);
            if (m_uniqueKeys.count(key))
            {
                // The name has already been registered...
                return false;
            }
            else
            {
                // The name has not been registered, so we add the item...
                m_items->push_back(item);
                m_uniqueKeys.insert(key);
                m_autoResetEvent.set();
                return true;
            }
        }

        // Gets the current contents of the vector, and clears the data being held.
        VecItemTypePtr getItems()
        {
            std::scoped_lock lock(m_mutex);
            auto result = m_items;
            m_items = std::make_shared<VecItemType>();
            m_uniqueKeys.clear();
            return result;
        }

        // Waits for data to be available and gets the current contents of the vector, 
        // and clears the data being held.
        VecItemTypePtr waitAndGetItems(int millisecondsTimeout)
        {
            auto secondsTimeout = millisecondsTimeout / 1000.0;
            m_autoResetEvent.waitOne(secondsTimeout);
            return getItems();
        }

        // Sets the auto-reset event so that a waitAndGetItems() call will complete before the timeout has elapsed.
        void wakeUp()
        {
            m_autoResetEvent.set();
        }

    // Private data...
    private:
        // Vector of items.
        VecItemTypePtr m_items;

        // Set of keys for registered unique items.
        using UniqueKeySet = std::set<UniqueKeyType>;
        UniqueKeySet m_uniqueKeys;

        // Mutex.
        std::mutex m_mutex;

        // Signals when new data is available...
        AutoResetEvent m_autoResetEvent;
    };
} // namespace

