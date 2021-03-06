#ifndef Hash_h
#define Hash_h

#include <memory>
#include <unordered_map>

#include "rct-config.h"
#include "rct/List.h"

template <typename Key, typename Value>
class Hash : public std::unordered_map<Key, Value>
{
public:
    Hash() : std::unordered_map<Key, Value>() {}
#ifndef HAVE_UNORDERED_MAP_MOVE_CONSTRUCTOR_WORKS
    Hash(Hash<Key, Value> &&other)
        : std::unordered_map<Key, Value>(std::forward<Hash<Key, Value> >(other))
    {
        other.std::unordered_map<Key, Value>::operator=(Hash<Key, Value>());
    }

    Hash(const Hash<Key, Value> &other) = default;
    Hash<Key, Value> &operator=(const Hash<Key, Value> &other) = default;
    Hash<Key, Value> &operator=(Hash<Key, Value> &&other)
    {
        std::unordered_map<Key, Value>::operator=(std::forward<Hash<Key, Value> >(other));
        other.std::unordered_map<Key, Value>::operator=(Hash<Key, Value>());
        return *this;
    }
#endif

    bool contains(const Key &t) const
    {
        return std::unordered_map<Key, Value>::find(t) != std::unordered_map<Key, Value>::end();
    }

    bool isEmpty() const
    {
        return !std::unordered_map<Key, Value>::size();
    }

    Value value(const Key &key, const Value &defaultValue = Value(), bool *ok = 0) const
    {
        typename std::unordered_map<Key, Value>::const_iterator it = std::unordered_map<Key, Value>::find(key);
        if (it == std::unordered_map<Key, Value>::end()) {
            if (ok)
                *ok = false;
            return defaultValue;
        }
        if (ok)
            *ok = true;
        return it->second;
    }

    void deleteAll()
    {
        typename std::unordered_map<Key, Value>::iterator it = std::unordered_map<Key, Value>::begin();
        while (it != std::unordered_map<Key, Value>::end()) {
            delete it->second;
            ++it;
        }
        std::unordered_map<Key, Value>::clear();
    }

    Value take(const Key &key, bool *ok = 0)
    {
        Value ret = Value();
        if (remove(key, &ret)) {
            if (ok)
                *ok = true;
        } else if (ok) {
            *ok = false;
        }
        return ret;
    }

    bool remove(const Key &t, Value *value = 0)
    {
        typename std::unordered_map<Key, Value>::iterator it = std::unordered_map<Key, Value>::find(t);
        if (it != std::unordered_map<Key, Value>::end()) {
            if (value)
                *value = it->second;
            std::unordered_map<Key, Value>::erase(it);
            return true;
        }
        if (value)
            *value = Value();
        return false;
    }


    int remove(std::function<bool(const Key &key)> match)
    {
        int ret = 0;
        typename std::unordered_map<Key, Value>::iterator it = std::unordered_map<Key, Value>::begin();
        while (it != std::unordered_map<Key, Value>::end()) {
            if (match(it->first)) {
                std::unordered_map<Key, Value>::erase(it++);
                ++ret;
            } else {
                ++it;
            }
        }
        return ret;
    }

    bool insert(const Key &key, const Value &value)
    {
        return std::unordered_map<Key, Value>::insert(std::make_pair(key, value)).second;
    }

    Value &operator[](const Key &key)
    {
        return std::unordered_map<Key, Value>::operator[](key);
    }

    const Value &operator[](const Key &key) const
    {
        assert(contains(key));
        return std::unordered_map<Key, Value>::find(key)->second;
    }

    Hash<Key, Value> &unite(const Hash<Key, Value> &other, int *count = 0)
    {
        typename std::unordered_map<Key, Value>::const_iterator it = other.begin();
        const auto end = other.end();
        while (it != end) {
            const Key &key = it->first;
            const Value &val = it->second;
            if (count) {
                auto cur = std::unordered_map<Key, Value>::find(key);
                if (cur == end || cur->second != val) {
                    ++*count;
                    std::unordered_map<Key, Value>::operator[](key) = val;
                }
            } else {
                std::unordered_map<Key, Value>::operator[](key) = val;
            }
            ++it;
        }
        return *this;
    }

    Hash<Key, Value> &subtract(const Hash<Key, Value> &other)
    {
        typename std::unordered_map<Key, Value>::iterator it = other.begin();
        while (it != other.end()) {
            std::unordered_map<Key, Value>::erase(*it);
            ++it;
        }
        return *this;
    }

    Hash<Key, Value> &operator+=(const Hash<Key, Value> &other)
    {
        return unite(other);
    }

    Hash<Key, Value> &operator-=(const Hash<Key, Value> &other)
    {
        return subtract(other);
    }

    int size() const
    {
        return std::unordered_map<Key, Value>::size();
    }

    typename std::unordered_map<Key, Value>::const_iterator constBegin() const
    {
        return std::unordered_map<Key, Value>::begin();
    }

    typename std::unordered_map<Key, Value>::const_iterator constEnd() const
    {
        return std::unordered_map<Key, Value>::end();
    }

    List<Key> keys() const
    {
        List<Key> keys;
        typename std::unordered_map<Key, Value>::const_iterator it = std::unordered_map<Key, Value>::begin();
        while (it != std::unordered_map<Key, Value>::end()) {
            keys.append(it->first);
            ++it;
        }
        return keys;
    }

    Set<Key> keysAsSet() const
    {
        Set<Key> keys;
        typename std::unordered_map<Key, Value>::const_iterator it = std::unordered_map<Key, Value>::begin();
        while (it != std::unordered_map<Key, Value>::end()) {
            keys.insert(it->first);
            ++it;
        }
        return keys;
    }

    List<Value> values() const
    {
        List<Value> values;
        typename std::unordered_map<Key, Value>::const_iterator it = std::unordered_map<Key, Value>::begin();
        while (it != std::unordered_map<Key, Value>::end()) {
            values.append(it->second);
            ++it;
        }
        return values;
    }
};

template <typename Key, typename Value>
inline const Hash<Key, Value> operator+(const Hash<Key, Value> &l, const Hash<Key, Value> &r)
{
    Hash<Key, Value> ret = l;
    ret += r;
    return ret;
}

template <typename Key, typename Value>
inline const Hash<Key, Value> operator-(const Hash<Key, Value> &l, const Hash<Key, Value> &r)
{
    Hash<Key, Value> ret = l;
    ret -= r;
    return ret;
}

#endif
