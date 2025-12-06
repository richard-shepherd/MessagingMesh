#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <concepts>
#include "Logger.h"

namespace MessagingMesh
{
    /// <summary>
    /// Concept for types that can be reset for reuse.
    /// </summary>
    template<typename T>
    concept Resettable = requires(T t)
    {
        { t.reset() } -> std::same_as<void>;
    };

    /// <summary>
    /// Thread-safe object pool that works with shared_ptr.
    /// 
    /// Objects are automatically returned to the pool when the last shared_ptr
    /// reference goes out of scope (via a custom deleter).
    /// 
    /// Usage:
    ///     // Define a pool for your type
    ///     using BufferPool = ObjectPool<Buffer>;
    ///     
    ///     // Acquire an object (returned to pool automatically when done)
    ///     auto pBuffer = BufferPool::acquire();
    /// 
    /// Requirements:
    ///     - T must be default constructible (can be private if ObjectPool<T> is a friend)
    ///     - T should satisfy Resettable concept (have a reset() method) for optimal reuse
    /// </summary>
    template<typename T>
    class ObjectPool
    {
    public:
        using Ptr = std::shared_ptr<T>;

        /// <summary>
        /// Acquires an object from the pool, or creates a new one if the pool is empty.
        /// The returned shared_ptr has a custom deleter that returns the object to the
        /// pool when the reference count reaches zero.
        /// </summary>
        static Ptr acquire()
        {
            T* pRaw = nullptr;

            {
                std::scoped_lock lock(s_mutex);
                if (!s_pool.empty())
                {
                    pRaw = s_pool.back();
                    s_pool.pop_back();
                }
            }

            if (pRaw)
            {
                // Reset the object for reuse if it supports reset()
                if constexpr (Resettable<T>)
                {
                    pRaw->reset();
                }
            }
            else
            {
                // Create a new object
                pRaw = new T();
            }

            // Return shared_ptr with custom deleter
            return Ptr(pRaw, &ObjectPool::release);
        }

        /// <summary>
        /// Returns the current number of objects in the pool (for diagnostics).
        /// </summary>
        static size_t poolSize()
        {
            std::scoped_lock lock(s_mutex);
            return s_pool.size();
        }

        /// <summary>
        /// Sets the maximum pool size. Objects beyond this limit are deleted
        /// rather than returned to the pool. Default is 256.
        /// </summary>
        static void setMaxPoolSize(size_t maxSize)
        {
            std::scoped_lock lock(s_mutex);
            s_maxPoolSize = maxSize;
        }

        /// <summary>
        /// Clears all objects from the pool, freeing their memory.
        /// </summary>
        static void clear()
        {
            std::scoped_lock lock(s_mutex);
            for (T* p : s_pool)
            {
                delete p;
            }
            s_pool.clear();
        }

    private:
        /// <summary>
        /// Custom deleter called when shared_ptr reference count reaches zero.
        /// Returns the object to the pool instead of deleting it.
        /// </summary>
        static void release(T* pObject)
        {
            if (!pObject) return;

            std::scoped_lock lock(s_mutex);

            if (s_pool.size() < s_maxPoolSize)
            {
                s_pool.push_back(pObject);  // Return to pool
            }
            else
            {
                Logger::info("ObjectPool: Pool full, deleting object");
                delete pObject;  // Pool is full, actually delete
            }
        }

        // Pool storage
        static inline std::vector<T*> s_pool;
        static inline std::mutex s_mutex;
        static inline size_t s_maxPoolSize = 100000;  // RSSTODO: Think about the pool size. Maybe have pools for different size objects / buffers?
    };

} // namespace