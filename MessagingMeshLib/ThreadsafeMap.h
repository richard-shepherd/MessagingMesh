#pragma once
#include <unordered_map>
#include <mutex>

namespace MessagingMesh
{
    /// <summary>
    /// Thread safe map for pointers.
    /// </summary>
    template<typename KeyType, typename ItemType>
    class ThreadsafeMap
    {
    // Public methods...
    public:
        // Inserts an item into the map.
        void insert(const KeyType& key, const ItemType& item)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_map.insert({ key, item });
        }

        // Returns the item for the key, or nullptr if it is not in the map.
        // Removes the item from the map.
        ItemType getAndRemove(const KeyType& key)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto it = m_map.find(key);
            if (it == m_map.end())
            {
                return nullptr;
            }
            auto item = it->second;
            m_map.erase(it);
            return item;
        }

    // Private data...
    private:
        // The map itself...
        std::unordered_map<KeyType, ItemType> m_map;

        // Mutex.
        std::mutex m_mutex;
    };
} // namespace


