#pragma once

#include "u_Array.h"

#include <unordered_map>

//
//  Simple least-recently-used cache
//


template <class V>
class Maker  // interface
{
public:
    virtual void make(V&) const = 0;
    virtual ~Maker() = default;
};


///
/// Assures that Maker has op()(What&)
///
template <class Maker, class What>
concept Making = requires(const Maker& that, What& what) {
    { that(what) };
};


template <class V, class Body> requires Making<Body,V>
class MakerT : public Maker<V>
{
public:
    const Body& body;
    MakerT(const Body& aBody) : body(aBody) {}
    virtual void make(V& v) const override { body(v); }
};


namespace detail {

    template <class K, class V>
    class LruCacheBase
    {
    public:
        LruCacheBase(size_t aCapacity) : fEntries(aCapacity) {}
        size_t capacity() const { return fEntries.size(); }
        size_t size() const { return fSize; }
    protected:
        struct Entry {
            K key;
            V value;
            Entry *prev = nullptr, *next = nullptr;
        };
        Array1d<Entry> fEntries;
        Entry *head = nullptr, *tail = nullptr;
        size_t fSize = 0;

        /// Extracts Entry from cache, does not erase
        void extract(Entry* entry);
        /// Puts Entry to the head of the cache
        void prepend(Entry* entry);
        /// Puts Entry to the very front
        void bump(Entry* entry);
    };

}   // namespace detail

///
/// @brief NdxOf
///   Assures that template Ndx is an index of key K and value V
///   * find returns iterator whose first is comparable with K, and second is approx. V&
///   * op[] for creation (≈V&) and erase for deletion are present somehow
/// @tparam K     key type
/// @tparam V     value type
/// @tparam Ndx   index template
///
template <class K, class V, template<class, class> class Ndx>
concept NdxOf = requires(Ndx<K,V>& ndx, const K& k, const V& v) {
    { ndx.find(k)->first == k };
    { ndx.find(k)->second = v };
    { ndx.erase(k) };
    { ndx[k] = v };
};


template<class K, class V>
using UoMap = std::unordered_map<K,V>;

///
/// @brief The LruCache class
///    Simple least-recently-used cache
///
/// @tparam K    key type
/// @tparam V    value type
/// @tparam Ndx  index template that checks whether we have the object
///
template <class K, class V, template<class, class> class Ndx = UoMap>
    requires NdxOf<K,V,Ndx>
class LruCache : public detail::LruCacheBase<K,V>
{
private:
    using Super = detail::LruCacheBase<K,V>;
public:
    using Super::Super;
    LruCache(size_t aCapacity);

    V& get(const K& key, const Maker<V>& maker);

    template <class Body> requires Making<Body,V>
        V& getT(const K& key, const Body& body)
            { return get(key, MakerT<V,Body>(body)); }

    using Super::size;
    using Super::capacity;
protected:
    using typename Super::Entry;
    using Super::fEntries;
    using Super::head;
    using Super::tail;
    using Super::fSize;
    using Super::extract;
    using Super::prepend;
    using Super::bump;
private:
    Ndx<K, typename Super::Entry*> ndx;
};


template<class K, class V>
void detail::LruCacheBase<K,V>::extract(Entry* entry)
{
    if (entry->next) {
        if (!tail)
            throw std::logic_error("[LruCache.extract] next SMTH, tail 0");
        entry->next->prev = entry->prev;
    } else {
        if (tail != entry)
            throw std::logic_error("[LruCache.extract] next=0, tail is not here");
        tail = entry->prev;
    }
    if (entry->prev) {
        if (!head)
            throw std::logic_error("[LruCache.extract] prev SMTH, head 0");
        entry->prev->next = entry->next;
    } else {
        if (head != entry)
            throw std::logic_error("[LruCache.extract] prev=0, head is not here");
        head = entry->prev;
    }
    entry->next = nullptr;
    entry->prev = nullptr;
}


template <class K, class V>
void detail::LruCacheBase<K,V>::prepend(Entry* entry)
{
    if (!head) {
        // Empty list
        if (tail)
            throw std::logic_error("[LruCache.prepend] head 0, tail SMTH");
        tail = entry;
        entry->next = nullptr;
    } else {
        // Non-empty list
        if (!tail)
            throw std::logic_error("[LruCache.prepend] head SMTH, tail 0");
        entry->next = head;
        head->prev = entry;
    }
    head = entry;
    entry->prev = nullptr;
}


template <class K, class V>
void detail::LruCacheBase<K,V>::bump(Entry* entry)
{
    if (entry != head) {
        extract(entry);
        prepend(entry);
    }
}


template<class K, class V, template<class, class> class Ndx>
    requires NdxOf<K,V,Ndx>
LruCache<K,V,Ndx>::LruCache(size_t aCapacity) : Super(aCapacity) {}


template <class K, class V, template<class, class> class Ndx>
    requires NdxOf<K,V,Ndx>
V& LruCache<K,V,Ndx>::get(const K& key, const Maker<V>& maker)
{
    auto it = ndx.find(key);
    if (it != ndx.end()) {
        // FOUND
        bump(it->second);
        return it->second->value;
    } else {
        // NOT FOUND
        Entry* entry;
        if (fSize >= capacity()) {
            if (!tail)
                throw std::logic_error("[LruCache.get] Full cache, no tail");
            entry = tail;
            extract(entry);
            ndx.erase(entry->key);
        } else {
            entry = &fEntries[fSize++];
        }
        entry->key = key;
        maker.make(entry->value);
        ndx[key] = entry;
        prepend(entry);
        return entry->value;
    }
}
