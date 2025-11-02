#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>
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

    void updatePosition(int key);
    void cacheInitialiser(int cap);
public:
    // Cache need not be persistent
    LRU_Cache();
    std::string get(int key);
    
    int create(int key, std::string value);

    int update(int key, std::string value);

    int remove(int key);

    void printCache();

    ~LRU_Cache();
};

#endif