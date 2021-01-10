#ifndef MAP_H
#define MAP_H

#include "list.h"
#include "optional.h"

namespace klib
{
    constexpr size_t BUCKETS = 32;

    template <typename K, typename V>
    struct keyvalue_t
    {
        K key;
        V value;
    };

    template <typename K, typename V>
    class map
    {
        //list<keyvalue_t<K, V>> values_[BUCKETS];
        keyvalue_t<K, V> values_[BUCKETS];

    private:
        uint64_t hash(uint64_t value)
        {
            return value % BUCKETS;
        }

    public:
        bool has_key(K key)
        {
            /*
            for (auto item : values_[key.hash()]) {
                if (key == item.key) {
                    return true;
                }
            }
            */

            return false;
        }

        void insert(K key, V value)
        {
            values_[key.hash() % BUCKETS] = keyvalue_t{key, move(value)};
        }

        void remove(K key)
        {
            //values_[key.hash()].remove(key);
        }

        auto at(K key) const
        {
            return values_[key.hash() % BUCKETS].value.get();
            /*
            decltype(
            for (auto item : values_[key.hash()]) {
                if (key == item.key) {
                    //if constexpr (is_same<class T, class U>
                    return item.value;
                }
            }

            return optional<V>();
            */
        }
    };
}

#endif // MAP_H
