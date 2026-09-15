#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
using namespace std;

class LRUCache {
private:
    struct Node {
        int key;
        int val;
        Node* prev;
        Node* next;
        Node(int key, int value){
            this->key = key;
            this->val = value;
        };
    };
    Node* head;
    Node* tail;
    int capacity;
    unordered_map<int, Node*> mp;

    mutex mtx;
    condition_variable cv;
    unordered_set<int> inProgress;

    void removeNode(Node* n) {
        n->prev->next = n->next;
        n->next->prev = n->prev;
    }

    void insertAtFront(Node* n) {
        n->next = head->next;
        head->next->prev = n;
        head->next = n;
        n->prev = head;
    }

    // internal versions: NO locking, caller must already hold the lock
    int getInternal(int key) {
        if (mp.find(key) != mp.end()) {
            Node* node = mp[key];
            removeNode(node);
            insertAtFront(node);
            return node->val;
        }
        return -1;
    }

    void putInternal(int key, int value) {
        if (mp.find(key) != mp.end()) {
            Node* node = mp[key];
            removeNode(node);
            node->val = value;
            insertAtFront(node);
            return;
        }
        if ((int)mp.size() == capacity) {
            Node* lru = tail->prev;
            mp.erase(lru->key);
            removeNode(lru);
            delete lru;
        }
        Node* newNode = new Node(key, value);
        insertAtFront(newNode);
        mp[key] = newNode;
    }

public:
    LRUCache (int capacity){
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
        this->capacity = capacity;
    }

    // public versions: lock, then call the internal one
    int get(int key) {
        lock_guard<mutex> lock(mtx);
        return getInternal(key);
    }

    void put(int key, int value) {
        lock_guard<mutex> lock(mtx);
        putInternal(key, value);
    }

    int slowFetch(int key) {
        this_thread::sleep_for(chrono::milliseconds(500));
        return key * 100;
    }

    int getOrCompute(int key) {
        unique_lock<mutex> lock(mtx);

        if (mp.find(key) != mp.end()) {
            return getInternal(key);
        }

        if (inProgress.count(key)) {
            cv.wait(lock, [this, key]() {
                return mp.find(key) != mp.end();
            });
            return getInternal(key);
        }

        inProgress.insert(key);
        lock.unlock();
        int value = slowFetch(key);
        lock.lock();

        putInternal(key, value);
        inProgress.erase(key);
        cv.notify_all();

        return value;
    }
};

int main(){
    LRUCache lru(5);
    auto worker = [&](int id) {
        int v = lru.getOrCompute(42);
        cout << "thread " << id << " got value " << v << endl;
    };
    thread t1(worker, 1);
    thread t2(worker, 2);
    thread t3(worker, 3);
    t1.join(); t2.join(); t3.join();
    return 0;
}