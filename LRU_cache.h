#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>

struct ListNode
{
    int key;
    std::string value; 
    ListNode* next;
    ListNode* prev;
    ListNode(int k){
        key = k;
        value = "";
        next = prev = nullptr;
    }
};

class LRU_Cache {
private:
    ListNode *head, *tail;
    std::unordered_map<int, ListNode*> mp;
    int capacity;
    std::mutex mutex_;

    void updatePosition(int key);
    void cacheInitialiser(int cap);
    void removeUnsafe(int key);
public:
    // Cache need not be persistent
    LRU_Cache(int cap=100);
    bool get(int key, std::string& value);
    
    bool create(int key, std::string value);

    bool update(int key, std::string value);

    bool remove(int key);

    // void printCache();

    ~LRU_Cache();
};

#endif
