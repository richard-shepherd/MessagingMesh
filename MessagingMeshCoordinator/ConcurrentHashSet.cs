using System.Collections.Concurrent;
using System.Collections.Generic;

namespace MessagingMeshCoordinator
{
    /// <summary>
    /// Thread-safe set implementation using ConcurrentDictionary.
    /// </summary>
    internal class ConcurrentHashSet<T> where T : notnull
    {
        private readonly ConcurrentDictionary<T, byte> m_dictionary = new();

        public void Add(T item) => m_dictionary.TryAdd(item, 0);

        public bool Remove(T item) => m_dictionary.TryRemove(item, out _);

        public bool Contains(T item) => m_dictionary.ContainsKey(item);

        public IEnumerable<T> Items => m_dictionary.Keys;
    }
}