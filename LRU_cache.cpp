#include "LRU_cache.h"

    void LRU_Cache::cacheInitialiser(int cap){
        head = new ListNode(-1);
        tail = new ListNode(-1);
        head->next = tail;
        tail->prev = head;
        capacity = cap;
    }

    LRU_Cache::LRU_Cache(){
        int cap;
        std::cout << "Enter the size of Cache : ";
        std::cin >> cap;
        cacheInitialiser(cap);
    }

    void LRU_Cache::updatePosition(int key){
        mp[key]->prev->next = mp[key]->next;
        mp[key]->next->prev = mp[key]->prev;
        
        ListNode *prev = tail->prev;
        mp[key]->next = tail;
        tail->prev = mp[key];
        mp[key]->prev = prev;
        prev->next = mp[key];
    }
    
    bool LRU_Cache::get(int key, std::string& value){
        if(mp.find(key)==mp.end()) return false;
        value = mp[key]->value;
        updatePosition(key);
        return true;
    }
    
    bool LRU_Cache::create(int key, std::string value){
        if(mp.find(key)!=mp.end()) return false;
        mp[key] = new ListNode(key);
        mp[key]->value = value;
        
        ListNode *prev = tail->prev;
        mp[key]->next = tail;
        tail->prev = mp[key];
        mp[key]->prev = prev;
        prev->next = mp[key];

        if(mp.size() > capacity) {
            remove(head->next->key);
        }

        return true;
    }
    
    bool LRU_Cache::update(int key, std::string value){
        if(mp.find(key)==mp.end()) return false;
        mp[key]->value = value;
        updatePosition(key);
        return true;
    }
    
    bool LRU_Cache::remove(int key){
        if(mp.find(key)==mp.end()) return false;
        mp[key]->prev->next = mp[key]->next;
        mp[key]->next->prev = mp[key]->prev;
        delete mp[key];
        mp.erase(key);
        return true;
    }

    void LRU_Cache::printCache(){
        std::cout<<capacity<<std::endl;
        ListNode *cur = head->next;
        while(cur and cur!=tail){
            std::cout<<cur->key<<" "<<cur->value<<std::endl;
            cur = cur->next;
        }
    }

    LRU_Cache::~LRU_Cache() {
        
        ListNode* cur = head;
        while(cur){
            // if(cur!=head and cur!=tail and outfile) outfile << cur->key <<" "<<cur->value<<"\n";
            ListNode *next = cur->next;
            delete cur;
            cur = next;
        }
    }



