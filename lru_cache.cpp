#include "lru_cache.h"

LRUCache::LRUCache(size_t capacity) : capacity_(capacity) {}

void LRUCache::put(int key, const std::string &value) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = map_.find(key);
    if (it != map_.end()) {
        // update and move to front
        it->second->second = value;
        items_.splice(items_.begin(), items_, it->second);
        map_[key] = items_.begin();
        return;
    }

    // insert new
    if (items_.size() >= capacity_) {
        // evict LRU
        auto last = items_.back();
        map_.erase(last.first);
        items_.pop_back();
    }
    items_.emplace_front(key, value);
    map_[key] = items_.begin();
}

std::optional<std::string> LRUCache::get(int key) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = map_.find(key);
    if (it == map_.end()) return std::nullopt;
    // move to front (most recently used)
    items_.splice(items_.begin(), items_, it->second);
    map_[key] = items_.begin();
    return it->second->second;
}

void LRUCache::remove(int key) {
    std::lock_guard<std::mutex> lock(mu_);
    auto it = map_.find(key);
    if (it == map_.end()) return;
    items_.erase(it->second);
    map_.erase(it);
}

size_t LRUCache::size() {
    std::lock_guard<std::mutex> lock(mu_);
    return items_.size();
}
