#ifndef LAB_2_HASH_TABLE_H
#define LAB_2_HASH_TABLE_H
#define DEFAULT_VALUE 1

#include<iostream>
#include<limits>
#include<string>
#include<cstring>
#include<vector>
#include<array>
#include<type_traits>
#include<fstream>

const size_t NODE_COUNT = 1024;

template<typename K>
struct BucketNode {
    BucketNode() : next(nullptr), value(0) {};
    BucketNode *next;
    K key;
    int value;
};

size_t gnu_hash(void const *const data, size_t const size) {
    char const *const chdata = (char const *const) data;
    size_t h = 5381;
    for (char const *ch = chdata; ch < chdata + size; ++ch) {
        h = h * 33 + (size_t) *ch;
    }
    return h;
}

template<typename K>
class UnorderedMap {
public:
    UnorderedMap();

    UnorderedMap(const UnorderedMap<K> &other);

    ~UnorderedMap();

    bool insert(const K &key, const int &value);

    bool erase(const K &key);

    [[nodiscard]] size_t size() const;

    void clear();

    std::vector<K> all_keys() const;

    [[nodiscard]] bool find(const K &key) const;

    [[nodiscard]] bool save(const std::string &f_name) const;

    bool load(const std::string &f_name);

    [[nodiscard]] std::array<K, 3> get_winners() const;

    bool operator==(const UnorderedMap<K> &other) const;

    void operator<<(const K &key);

    const int &operator[](const K &key) const; 

private:
    int *get(const K &key) const;

    [[nodiscard]] size_t get_hash(const K &key) const;

    K find_max_lower_than(int value) const;

    BucketNode<K> *m_buckets[NODE_COUNT];

    size_t m_size;
};

template<typename K>
UnorderedMap<K>::UnorderedMap() : m_size(0) {
    // fill all bytes with m_buckets with 0
    memset(m_buckets, 0, sizeof(m_buckets));
}

template<typename K>
UnorderedMap<K>::~UnorderedMap() {
    clear();
}

template<typename K>
UnorderedMap<K>::UnorderedMap(const UnorderedMap<K> &other) :
        m_size(other.size()) {
    for (auto *bucket: other.m_buckets) {
        auto *cur_node = bucket;
        while (cur_node != nullptr) {
            insert(cur_node->key, cur_node->value);
            cur_node = cur_node->next;
        }
    }
}

template<typename K>
bool UnorderedMap<K>::insert(const K &key, const int &value) {
    size_t hash = get_hash(key);
    if (get(key) != nullptr)
        return false;
    auto *temp_node = new BucketNode<K>();
    temp_node->key = key;
    temp_node->value = value;
    temp_node->next = m_buckets[hash];
    m_buckets[hash] = temp_node;
    m_size++;
    return true;
}

template<typename K>
int *UnorderedMap<K>::get(const K &key) const {
    size_t hash = get_hash(key);
    BucketNode<K> *cur_node = m_buckets[hash];
    while (cur_node != nullptr) {
        if (cur_node->key == key)
            return &(cur_node->value);
        cur_node = cur_node->next;
    }
    return nullptr;
}

template<typename K>
size_t UnorderedMap<K>::get_hash(const K &key) const {
    return gnu_hash(key.data(), key.size()) % NODE_COUNT;
}

template<typename K>
void UnorderedMap<K>::clear() {
    for (auto *&bucket: m_buckets) {
        auto *cur_node = bucket;
        while (cur_node != nullptr) {
            auto *next_node = cur_node->next;
            delete cur_node;
            cur_node = next_node;
        }
        bucket = nullptr;
    }
    m_size = 0;
}

template<typename K>
std::vector<K> UnorderedMap<K>::all_keys() const {
    std::vector<K> return_list;
    for (auto *bucket: m_buckets){
        auto *cur_node = bucket;
        while (cur_node != nullptr){
            return_list.push_back(cur_node->key);
            cur_node = cur_node->next;
        }
    }
    return return_list;
}

template<typename K>
size_t UnorderedMap<K>::size() const {
    return m_size;
}

template<typename K>
bool UnorderedMap<K>::save(const std::string &f_name) const {
    std::ofstream f_out(f_name);
    for (auto *bucket: m_buckets) {
        auto *cur_node = bucket;
        while (cur_node != nullptr) {
            if (!(f_out << cur_node->key << ' ' << cur_node->value << std::endl))
                return false;
            cur_node = cur_node->next;
        }
    }
    return true;
}

template<typename K>
bool UnorderedMap<K>::load(const std::string &f_name) {
    std::ifstream f_in(f_name);
    if (!f_in.is_open())
        return false;
    while (!f_in.eof()) {
        K key;
        int value;
        if (f_in >> key >> value)
            this->insert(key, value);
    }
    return true;
}

template<typename K>
bool UnorderedMap<K>::operator==(const UnorderedMap<K> &other) const {
    if (m_size != other.m_size)
        return false;
    for (auto *bucket: m_buckets) {
        auto *cur_node = bucket;
        while (cur_node != nullptr) {
            auto *p = other.get(cur_node->key);
            if (p == nullptr)
                return false;
            if (*p != cur_node->value)
                return false;
            cur_node = cur_node->next;
        }
    }
    return true;
}

template<typename K>
void UnorderedMap<K>::operator<<(const K &key) {
    auto *p = get(key);
    if (p == nullptr) {
        insert(key, DEFAULT_VALUE);
    } else {
        // инкремент значения из полученной структуры
        ++*p;
    }
}

template<typename K>
const int &UnorderedMap<K>::operator[](const K &key) const {
    auto *p = get(key);
    if (p == nullptr)
        throw std::out_of_range("Invalid key");
    return *p;
}

template<typename K>
bool UnorderedMap<K>::erase(const K &key) {
    size_t hash = get_hash(key);
    auto *cur_node = m_buckets[hash];
    BucketNode<K> *prev_node = nullptr;
    while (cur_node != nullptr) {
        if (cur_node->key == key) {
            if (prev_node == nullptr) {
                m_buckets[hash] = cur_node->next;
            } else {
                prev_node->next = cur_node->next;
            }
            delete cur_node;
            return true;
        }
        prev_node = cur_node;
        cur_node = cur_node->next;
    }
    return false;
}

template<typename K>
bool UnorderedMap<K>::find(const K &key) const {
    return get(key) != nullptr;
}

template<typename K>
K UnorderedMap<K>::find_max_lower_than(int value) const {
    BucketNode<K> *max_node = nullptr;
    for (auto *bucket: m_buckets) {
        auto *cur_node = bucket;
        while (cur_node != nullptr) {
            if (max_node == nullptr) {
                max_node = cur_node;
            } else {
                if (max_node->value < cur_node->value) {
                    if (cur_node->value < value) {
                        max_node = cur_node;
                    }
                }
            }
            cur_node = cur_node->next;
        }
    }
    return max_node->key;
}

template<typename K>
std::array<K, 3> UnorderedMap<K>::get_winners() const {
    if (m_size >= 3) {
    K key1, key2, key3;
    key1 = find_max_lower_than(std::numeric_limits<int>::max());
    int *val1 = get(key1);
    key2 = find_max_lower_than(*val1);
    int *val2 = get(key2);
    key3 = find_max_lower_than(*val2);
    return {key1, key2, key3};
    } else {
        throw std::logic_error("Not enough members!");
    }
}

#endif //LAB_2_HASH_TABLE_H
