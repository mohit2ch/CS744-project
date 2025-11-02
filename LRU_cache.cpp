#include "LRU_cache.h"

    void LRU_Cache::cacheInitialiser(int cap){
        head = new ListNode(-1);
        tail = new ListNode(-1);
        head->next = tail;
        tail->prev = head;
        capacity = cap;
    }

    LRU_Cache::LRU_Cache(){
        std::ifstream infile("lru_cache.txt");
        if(!infile.good()){
            int cap;
            std::cout << "Cache file not found. Enter the size of Cache : ";
            std::cin >> cap;

            cacheInitialiser(cap);
            std::ofstream outfile("lru_cache.txt");
            if(!outfile) {
                std::cerr << "Error creating cache file. Switching to session wide LRU cache\n";
                return;
            }

            outfile << cap << std::endl;
            outfile.close();
        }
        else {
            int cap;
            infile >> cap;
            cacheInitialiser(cap);

            int key;
            std::string value;
            while(infile >> key >> value){
                create(key, value);
            }
            std::cout << "Cache loaded from file.\n";
        }
        infile.close();
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
    
    std::string LRU_Cache::get(int key){
        if(mp.find(key)==mp.end()) return "-1";
        std::string res = mp[key]->value;
        updatePosition(key);
        return res;
    }
    
    int LRU_Cache::create(int key, std::string value){
        if(mp.find(key)!=mp.end()) return -1;
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

        return 1;
    }
    
    int LRU_Cache::update(int key, std::string value){
        if(mp.find(key)==mp.end()) return -1;
        mp[key]->value = value;
        updatePosition(key);
        return 1;
    }
    
    int LRU_Cache::remove(int key){
        if(mp.find(key)==mp.end()) return -1;
        mp[key]->prev->next = mp[key]->next;
        mp[key]->next->prev = mp[key]->prev;
        delete mp[key];
        mp.erase(key);
        return 1;
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
        std::ofstream outfile("lru_cache.txt");
        std::cout << "Storing Cache into persistent storage\n";
        outfile << capacity << "\n";
        ListNode* cur = head;
        while(cur){
            if(cur!=head and cur!=tail and outfile) outfile << cur->key <<" "<<cur->value<<"\n";
            ListNode *next = cur->next;
            delete cur;
            cur = next;
        }
    }



