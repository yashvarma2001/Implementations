#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <iterator>

class LRUCache {
private:
    // 1. Capacity of the cache
    int capacity;

    // 2. Doubly Linked List: Stores {key, value} pairs.
    // Most recently used items go to the front; least recently used stays at the back.
    std::list<std::pair<int, int>> cache_list;

    // 3. Hash Map: Maps Key -> Iterator (pointer) to the node in the list.
    // This allows us to jump directly to a node in the list in O(1) time.
    std::unordered_map<int, std::list<std::pair<int, int>>::iterator> cache_map;

public:
    // Constructor
    LRUCache(int capacity) {
        // TODO: Initialize capacity
    }

    // Returns value if key exists, otherwise -1.
    // SIDE EFFECT: Must move the accessed key to the front of the list (mark as recently used).
    int get(int key) {
        // TODO: Implement logic
        // 1. Check if key exists in map.
        // 2. If no, return -1.
        // 3. If yes, move the node to the front of the list (splice).
        // 4. Return the value.
        return -1;
    }

    // Updates value if key exists, or inserts new key-value pair.
    // SIDE EFFECT: If capacity is exceeded, evict the Least Recently Used (back of list).
    void put(int key, int value) {
        // TODO: Implement logic
        // 1. If key exists: Update value, move to front.
        // 2. If key does not exist:
        //    a. Check if cache is full (size == capacity).
        //    b. If full, remove the last element from list and map.
        //    c. Insert new element at front of list.
        //    d. Update map.
    }
};


int main() {
    // Test code to verify your implementation
    LRUCache cache(2); // Capacity 2

    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << "Get 1: " << cache.get(1) << " (Expected: 1)" << std::endl; // returns 1
    
    cache.put(3, 3); // evicts key 2
    std::cout << "Get 2: " << cache.get(2) << " (Expected: -1)" << std::endl; // returns -1 (not found)
    
    cache.put(4, 4); // evicts key 1
    std::cout << "Get 1: " << cache.get(1) << " (Expected: -1)" << std::endl; // returns -1 (not found)
    std::cout << "Get 3: " << cache.get(3) << " (Expected: 3)" << std::endl; // returns 3
    std::cout << "Get 4: " << cache.get(4) << " (Expected: 4)" << std::endl; // returns 4

    return 0;
}