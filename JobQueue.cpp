#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept> // Required for "invalid_argument"

using namespace std;

class SimpleQueue {
private:
    queue<int> q;
    int capacity;
    mutex mtx;
    condition_variable cv; /* A Condition Variable is a tool that allows a thread 
                            to sleep until a specific situation changes, 
                            instead of constantly checking (which wastes CPU).
                            */ 

public:
    SimpleQueue(int cap) {
        if (cap <= 0) {
            throw invalid_argument("Capacity must be > 0");
        }
        this->capacity = cap;
    }

    // PRODUCER (Add Job)
    void add(int item) {
        // unique_lock - Can be locked and unlocked manually multiple times.
        unique_lock<mutex> lock(mtx);
        // State: LOCKED. (You have the key).

        // 1. Wait if Full
        while (q.size() >= capacity) {
            cv.wait(lock);
            // Step A (Going to sleep): It Unlocks (Releases the key so others can use it).
            // Step B (Sleeping): It waits.
            // Step C (Waking up): It RE-LOCKS (Grabs the key back).
        }
        // After while State: LOCKED.

        // 2. Add
        q.push(item);

        // 3. Wake up Consumers
        cv.notify_all();
    } // State: UNLOCKED.

    // CONSUMER (Take Job)
    int take() {
        unique_lock<mutex> lock(mtx);

        // 1. Wait if Empty
        while (q.empty()) {
            cv.wait(lock);
        }

        // 2. Take
        int item = q.front();
        q.pop();

        // 3. Wake up Producers
        cv.notify_all();
        
        return item;
    }
};