#include <functional>
#include <list>
#include <vector>
#include <iterator>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
 private:
    size_t Size;
    size_t Capacity;
    Hash hasher;
    std::list<std::pair<const KeyType, ValueType>> Elements;
    std::vector<decltype(Elements.begin())> Pointers;

 public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>:: iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>:: const_iterator;

    HashMap(const Hash& Hasher = Hash()): Size(0), Capacity(1), hasher(Hasher) {
        Pointers.resize(Capacity, Elements.end());
    }

    void Rebuild() {
        Capacity *= 2;
        Pointers.clear();
        Pointers.resize(Capacity, Elements.end());
        auto CopyOfElements = Elements;
        Elements.clear();
        Size = 0;
        for (auto x: CopyOfElements) {
            insert(x);
        }
    }

    size_t getIndex(const KeyType& key) const {
        return hasher(key) % Capacity;
    }

    iterator begin() {
        return Elements.begin();
    }

    const_iterator begin() const {
        return Elements.begin();
    }

    iterator end() {
        return Elements.end();
    }

    const_iterator end() const {
        return Elements.end();
    }

    size_t size() const {
        return Size;
    }

    bool empty() const {
        return (0 == Size);
    }

    Hash hash_function() const {
        return hasher;
    }

    iterator find(const KeyType& key) {
        size_t index = getIndex(key);
        if (Pointers[index] == end()) {
            return end();
        }
        iterator it = Pointers[index];
        while (it != end() && getIndex(it->first) == index) {
            if (it->first == key) {
                return it;
            }
            ++it;
        }
        return end();
    }

    const_iterator find(const KeyType& key) const {
        size_t index = getIndex(key);
        if (Pointers[index] == end()) {
            return end();
        }
        auto it = Pointers[index];
        while (it != end() && getIndex(it->first) == index) {
            if (it->first == key) {
                return it;
            }
            ++it;
        }
        return end();
    }

    iterator insert(const std::pair<KeyType, ValueType>& element) {
        if (Size == Capacity) {
            Rebuild();
        }
        const_iterator searchIter = find(element.first);
        if (searchIter != end()) {
            return end();
        }
        Size += 1;
        size_t index = getIndex(element.first);
        if (Pointers[index] == end()) {
            iterator returnIter = Elements.insert(Elements.end(), element);
            Pointers[index] = returnIter;
            return returnIter;
        } else {
            iterator nextBegin = Elements.insert(Pointers[index], element);
            Pointers[index] = nextBegin;
            return nextBegin;
        }
    }

    void erase(const KeyType& key) {
        iterator searchIter = find(key);
        if (searchIter != end()) {
            size_t index = getIndex(key);
            if (searchIter == Pointers[index]) {
                iterator nextIter = searchIter;
                nextIter++;
                if (nextIter == end() || getIndex(nextIter->first) != index) {
                    Pointers[index] = end();
                } else {
                    Pointers[index] = nextIter;
                }
            }
            Elements.erase(searchIter);
            Size -= 1;
        }
    }

    template <typename Iter>
    HashMap(Iter first, Iter last, const Hash& Hasher = Hash()): Size(0), Capacity(1), hasher(Hasher) {
        Pointers.resize(Capacity, Elements.end());
        while (first != last) {
            insert(*first);
            first++;
        }
    }

    HashMap(const std::initializer_list<std::pair<KeyType, ValueType>>& List,
            const Hash& Hasher = Hash()) : HashMap(List.begin(), List.end(), Hasher) {};

    ValueType& operator[] (const KeyType& key) {
        iterator searchIter = find(key);
        if (searchIter == end()) {
            iterator insertIter = insert(std::make_pair(key, ValueType()));
            return insertIter->second;
        }
        return searchIter->second;
    }


    const ValueType& at(const KeyType& key) const {
        const_iterator searchIter = find(key);
        if (searchIter == end()) {
            throw std::out_of_range("Out of range");
        }
        return searchIter->second;
    }

    void clear() {
        Elements.clear();
        Size = 0;
        std::fill(Pointers.begin(), Pointers.end(), end());
    }

    HashMap(const HashMap& M) {
        *this = M;
    }

    HashMap& operator= (const HashMap& M) {
        Size = 0;
        Capacity = 1;
        auto CopyOfElements = M.Elements;
        Elements.clear();
        Pointers.clear();
        Pointers.resize(Capacity, Elements.end());
        hasher = M.hasher;
        for (auto x: CopyOfElements) {
            insert(x);
        }
        return *this;
    }

    size_t capacity() const {
        return Capacity;
    }
};
