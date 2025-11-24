#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <list>
#include <string>
#include <unordered_map>
#include <mutex>
#include <optional>

class LRUCache {
public:
    explicit LRUCache(size_t capacity);

    // Put key-value (insert/update). If cache is full, evict LRU.
    void put(int key, const std::string &value);

    // Get value if present, else nullopt.
    std::optional<std::string> get(int key);

    // Remove key if present.
    void remove(int key);

    // Return current size
    size_t size();

private:
    using Item = std::pair<int, std::string>;
    size_t capacity_;
    std::list<Item> items_; // front = most-recent, back = least-recent
    std::unordered_map<int, std::list<Item>::iterator> map_;
    std::mutex mu_;
};
#endif // LRU_CACHE_H
