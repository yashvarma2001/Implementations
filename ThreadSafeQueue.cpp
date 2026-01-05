#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace std;
template <typename T>

class ThreadSafeQueue {
private:
    queue<T> q;
    int capacity;
    mutex mtx;

    /* A Condition Variable is a tool that allows a thread 
    to sleep until a specific situation changes, 
    instead of constantly checking (which wastes CPU).
    */ 

    // Optimization: Split the waiting lines
    condition_variable notFull;  // Producers wait here
    condition_variable notEmpty; // Consumers wait here

    // Helper for Timestamp Logging
    long long getCurrentTime() {
        return chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    SimpleQueue(int cap) {
        if (cap <= 0) {
            throw invalid_argument("Capacity must be > 0");
        }
        this->capacity = cap;
    }

    // PRODUCER (Add Job)
    void add(T item) {
        // unique_lock - Can be locked and unlocked manually multiple times.
        unique_lock<mutex> lock(mtx);
        // State: LOCKED. (You have the key).

        // 1. Wait if Full
        while (q.size() >= capacity) {
            notFull.wait(lock);
            // Step A (Going to sleep): It Unlocks (Releases the key so others can use it).
            // Step B (Sleeping): It waits.
            // Step C (Waking up): It RE-LOCKS (Grabs the key back).
        }
        // After while State: LOCKED.

        // 2. Add
        q.push(item);
        if (q.size() == capacity) {
            cout << "[LOG] Queue FULL at " << getCurrentTime() << endl;
        }

        // 3. Wake up Consumers
        notEmpty.notify_one();
    } // State: UNLOCKED.

    // CONSUMER (Take Job)
    T take() {
        unique_lock<mutex> lock(mtx);

        // 1. Wait if Empty
        while (q.empty()) {
            notEmpty.wait(lock);
        }

        // 2. Take
        int item = q.front();
        q.pop();

        if (q.empty()) {
            cout << "[LOG] Queue EMPTY at " << getCurrentTime() << endl;
        }

        // 3. Wake up one Producers
        notFull.notify_one();
        
        return item;
    }
};

int main() {
    ThreadSafeQueue<string> myQueue(2);

    vector<thread> threads;

    // Producer (Adds Strings)
    threads.push_back(thread([&]() {
        vector<string> messages = {"Job_A", "Job_B", "Job_C", "Job_D"};
        for (const string& msg : messages) {
            cout << "Adding: " << msg << endl;
            myQueue.add(msg);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }));

    // Consumer (Takes Strings)
    threads.push_back(thread([&]() {
        for (int i = 0; i < 4; i++) {
            string s = myQueue.take();
            cout << "Took: " << s << endl;
            this_thread::sleep_for(chrono::milliseconds(150));
        }
    }));

    for (auto& t : threads) t.join();
    return 0;
}