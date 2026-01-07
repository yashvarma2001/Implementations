#include <unordered_map>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class LRUCache {
private:
    mutex mtx; 

    struct Node {
        int key;
        int val;
        Node* next;
        Node* prev;
        Node(int k, int v) : key(k), val(v), next(nullptr), prev(nullptr) {}
    };

    unordered_map<int, Node*> mp;
    int capacity;
    Node* head;
    Node* tail;

    void add(Node* node) {
        Node* prevNode = tail->prev;
        node->next = tail;
        node->prev = prevNode;
        prevNode->next = node;
        tail->prev = node;
    }

    void remove(Node* node) {
        Node* prevNode = node->prev;
        Node* nextNode = node->next;
        prevNode->next = nextNode;
        nextNode->prev = prevNode;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }

    int get(int key) {
        lock_guard<mutex> lock(mtx);

        if (mp.find(key) != mp.end()) {
            Node* node = mp[key];
            remove(node);
            add(node);
            return node->val;
        }
        return -1;
    }

    void put(int key, int value) {
        lock_guard<mutex> lock(mtx);

        if (mp.find(key) != mp.end()) {
            Node* node = mp[key];
            remove(node);
            node->val = value;
            add(node);
            return;
        }

        if (mp.size() == capacity) {
            Node* lru = head->next;
            mp.erase(lru->key);
            remove(lru);
            delete lru;
        }

        Node* newNode = new Node(key, value);
        add(newNode);
        mp[key] = newNode;
    }
};

void testSafe() {
    LRUCache cache(5);

    auto writer = [&]() {
        for (int i = 0; i < 1000; i++) {
            cache.put(i, i * 10);
        }
    };

    auto reader = [&]() {
        for (int i = 0; i < 1000; i++) {
            cache.get(i);
        }
    };

    thread t1(writer);
    thread t2(reader);

    t1.join();
    t2.join();

    std::cout << "Success! No crashes." << std::endl;
}

int main() {
    testSafe();
    return 0;
}