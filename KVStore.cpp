#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <mutex>

using namespace std;

// "Many Readers, One Writer" pattern
class ThreadSafeKVStore {
private:
    unordered_map<string, string> store;
    mutable shared_mutex rwLock;
    /*
        In C++, get() is a const function (meaning "I promise not to change any variables").

        The Conflict: But locking a mutex technically changes the mutex (from Unlocked → Locked). 
        The compiler will yell at you.

        The Fix: mutable acts as a waiver. 
        It tells the compiler: "It is okay to change this specific variable (rwLock) even inside a const function."
    */

public:
    // READER: Shared Lock (Many threads can read at once)
    string get(string key) const {
        shared_lock lock(rwLock);
        /*
            The Special Tool: shared_mutex
            Normal mutex: Only 1 thread can enter, period. (Too slow for reads).

            shared_mutex:
            Shared Mode: Allows unlimited threads to enter (as long as nobody is writing).
            Exclusive Mode: Allows only 1 thread to enter (stops everyone else).
        */
        if (store.find(key)!=store.end()) {
            return store.at(key);
        }
        return "";
    }

    // WRITER: Unique Lock (Only one thread can write)
    void put(string key, string value) {
        unique_lock lock(rwLock);
        store[key] = value;
    }

    // ATOMIC Check-Then-Act
    bool putIfAbsent(string key, string value) {
        unique_lock lock(rwLock); // Must be exclusive
        if (store.find(key) == store.end()) {
            store[key] = value;
            return true;
        }
        return false;
    }
};