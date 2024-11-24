#ifndef SAFE_MAP_H
#define SAFE_MAP_H

#include <map>
#include <mutex>
#include <iostream>
#include "safeVector.h"


template <class K, class V, class Compare = std::less<K>, class Allocator = std::allocator<std::pair<const K, V> > >
class SafeMap {
    private:
        std::map<K, V, Compare, Allocator> safe_map;
        std::mutex mut;

    public:
        void set(K key, V value) {
            std::lock_guard<std::mutex> lk(this->mut);
            this->safe_map[key] = value;
        }

        V & get(K key) {
            std::lock_guard<std::mutex> lk(this->mut);
            return this->safe_map[key];
        }

        bool empty() {
            std::lock_guard<std::mutex> lk(this->mut);
            return this->safe_map.empty();
        }

        size_t len(void)
        {
            std::lock_guard<std::mutex> lk(this->mut);
            size_t size = safe_map.size();;
            return size;
        }

        safeVector<K> getAllKeys()
        {
            std::lock_guard<std::mutex> lk(this->mut);
            safeVector<K> keys = {};
            for (auto const& element : this->safe_map) 
            {
                keys.push(element.first);
            }
            return keys;
        }
};

#endif