# CPP Cheatsheet

## **1. The Essential Setup (Headers)**

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;
```

## 2. Locks and Mutexes

In an interview, it is critical to distinguish between the **Primitive** (the actual lock object) and the **Wrapper** (the RAII object that manages the lock automatically).

### Part 1: The Primitives (The "Box")

These are the actual objects you declare in your class.

### 1. `std::mutex` (The Standard)

- **What is it?** The most basic locking primitive. Only one thread can own it at a time.
- **Behavior:** If Thread A has it, Thread B sleeps until Thread A releases it.
- **Use Case:** 99% of your interview problems (Logger, Connection Pool, Queue).

### 2. `std::shared_mutex` (The Read-Write Lock)

- **What is it?** Allows **multiple** threads to read at the same time, but only **one** thread to write.
- **Behavior:**
    - *Reader Mode:* "I want to read." (Allowed if no one is writing).
    - *Writer Mode:* "I want to write." (Blocks everyone else).
- **Use Case:** A Cache or DNS lookup where 1,000 threads read and only 1 updates.

### 3. `std::recursive_mutex` (The Re-entrant Lock)

- **What is it?** Allows the **same thread** to lock the same mutex multiple times without crashing (deadlocking itself).
- **Behavior:** You must unlock it as many times as you locked it.
- **Use Case:** Recursive functions. **Warning:** Avoid in interviews. It usually implies bad design.

---

### Part 2: The Wrappers (The "Keys")

You almost **never** call `.lock()` manually. You use these wrappers to handle it for you.

### 1. `std::lock_guard<mutex>`

- **Speed:** Fastest.
- **Flexibility:** Low.
- **Mechanism:** Locks in constructor, unlocks in destructor. You cannot manually unlock it early.
- **Interview Rule:** Use this by default for simple critical sections.

```cpp
void simple() {
    lock_guard<mutex> lock(mtx); 
    // Protected code
} // Unlocks here automatically
```

### 2. `std::unique_lock<mutex>`

- **Speed:** Slightly slower (has a boolean flag inside).
- **Flexibility:** High.
- **Mechanism:** Can be manually unlocked/relocked. Can be moved to other scopes.
- **Interview Rule:** Mandatory for **Condition Variables** (`cv.wait`) or if you need to unlock early.

```cpp
void complex() {
    unique_lock<mutex> lock(mtx);
    
    // 1. Unlocking early to do slow work
    lock.unlock(); 
    doHeavyComputation(); 
    lock.lock(); 
    
    // 2. Required for CV
    cv.wait(lock);
}
```

### 3. `std::shared_lock<shared_mutex>`

- **Use with:** `std::shared_mutex`.
- **Mechanism:** Acquires the lock in **Shared (Read)** mode. Multiple threads can hold this simultaneously.

```cpp
void reader() {
    shared_lock<shared_mutex> lock(rw_mtx);
    cout << data << endl; // Multiple threads can be here at once
}
```

---

### Part 3: Cheat Sheet - When to use what?

| **Scenario** | **Primitive** | **Wrapper** | **Why?** |
| --- | --- | --- | --- |
| **Simple Protection** | `std::mutex` | `std::lock_guard` | Fastest, simplest, impossible to forget unlock. |
| **Waiting (CV)** | `std::mutex` | `std::unique_lock` | `cv.wait()` requires `unique_lock` to unlock while waiting. |
| **Manual Unlock** | `std::mutex` | `std::unique_lock` | If you need to release the lock early (e.g., before file I/O). |
| **Read-Heavy Cache** | `std::shared_mutex` | `std::shared_lock` (Reader)
`std::unique_lock` (Writer) | Allows parallel reads for performance. |

---

### Part 4: Code Examples

### A. Standard Mutual Exclusion (90% of cases)

```cpp
mutex mtx;

void addToQueue(int x) {
    lock_guard<mutex> lock(mtx); // Locks immediately
    queue.push(x);
} // Unlocks automatically
```

### B. The Read-Write Pattern (Optimisation)

Use this if the interviewer asks: *"This is too slow because only one person can read the configuration at a time."*

```cpp
#include <shared_mutex>

shared_mutex rw_mtx;
int configData = 0;

// Multiple threads can run this AT THE SAME TIME
int readConfig() {
    shared_lock<shared_mutex> lock(rw_mtx); 
    return configData;
}

// Only ONE thread can run this (blocks all readers)
void updateConfig(int newVal) {
    unique_lock<shared_mutex> lock(rw_mtx);
    configData = newVal;
}
```

### C. The "Wait" Pattern (Condition Variables)

```cpp
mutex mtx;
condition_variable cv;

void worker() {
    unique_lock<mutex> lock(mtx); // MUST be unique_lock
    cv.wait(lock); // Temporarily unlocks 'lock' while sleeping
}
```

## **3. The "Background Service" Pattern**

Use when: "Implement a system that runs forever" (Logger, Token Refill, Scheduler).

Key Features: Thread in Class, wait_for Timeout, Graceful Shutdown.

```cpp
class BackgroundService {
private:
    // 1. Core Data
    queue<string> buffer;
    
    // 2. Threading Tools
    mutex mtx;
    condition_variable cv;
    thread workerThread;
    bool running; // Control flag

    // 3. The Infinite Loop (The "Brain")
    void workerLoop() {
        while (true) {
            unique_lock<mutex> lock(mtx);
            
            // WAIT LOGIC:
            // Wake up if: (Condition Met) OR (Timeout) OR (Stopping)
            // 'wait_for' returns false on timeout, but we don't care.
            // We just wake up and check the buffer.
            cv.wait_for(lock, chrono::seconds(3), [this] {
                return buffer.size() >= 5 || !running;
            });

            // EXIT LOGIC:
            // Die ONLY if we are stopped AND the work is finished.
            if (!running && buffer.empty()) {
                break; // Kills the thread
            }

            // WORK LOGIC:
            // Process whatever is in the buffer (even if it was just a timeout)
            while (!buffer.empty()) {
                cout << "Processing: " << buffer.front() << endl;
                buffer.pop();
            }
        }
    }

public:
    // CONSTRUCTOR: Start the thread
    BackgroundService() {
        // Java-Style Initialization
        this->running = true;
        this->workerThread = thread(&BackgroundService::workerLoop, this);
    }

    // DESTRUCTOR: Stop the thread safely
    ~BackgroundService() {
        {
            lock_guard<mutex> lock(mtx);
            this->running = false; // 1. Signal stop
        }
        cv.notify_one();           // 2. Wake up (Cancel the 3s timeout)
        
        if (workerThread.joinable()) {
            workerThread.join();   // 3. Wait for it to die
        }
    }

    // PUBLIC API
    void addWork(string item) {
        lock_guard<mutex> lock(mtx);
        buffer.push(item);
        
        // Optional: Trigger immediately if full
        if (buffer.size() >= 5) {
            cv.notify_one();
        }
    }
};
```

## **4. The "Task Runner" Pattern (Main Function)**

Use when: "Simulate X users" or "Test your code".

Key Features: vector<thread>, Lambda capture [&], Loop, Join.

```cpp
int main() {
    // 1. SETUP
    BackgroundService system;
    vector<thread> pool;

    // 2. EXECUTE (Scale to N threads)
    for(int i = 0; i < 10; i++) {
        pool.push_back(thread([&, i]() {
            // Simulate User Behavior
            system.addWork("User " + to_string(i));
            
            // Optional: Simulate user thinking time
            this_thread::sleep_for(chrono::milliseconds(100));
        }));
    }

    // 3. FINISH (Wait for all users to be done)
    for(auto& t : pool) {
        if(t.joinable()) t.join();
    }

    cout << "All users finished. Destructor will handle cleanup." << endl;
    return 0;
}
```

## **5. Timer & Time Syntax Cheat Sheet**

Everything you need for Timeouts and Rate Limiters.

### **A. Sleeping (Delays)**

```cpp
// Sleep for 500 milliseconds
this_thread::sleep_for(chrono::milliseconds(500));

// Sleep for 2 seconds
this_thread::sleep_for(chrono::seconds(2));
```

### **B. Waiting with Timeout**

```cpp
unique_lock<mutex> lock(mtx);

// Wait up to 3 seconds.
// Returns TRUE if condition met.
// Returns FALSE if 3 seconds passed (Timeout).
bool result = cv.wait_for(lock, chrono::seconds(3), [this]{
    return !buffer.empty();
});
```

### **C. Getting Current Timestamp (For Rate Limiters)**

```cpp
// Get current time in Milliseconds (long long)
auto now = chrono::system_clock::now();
long long currentTimeMs = chrono::duration_cast<chrono::milliseconds>(
    now.time_since_epoch()
).count();
```

## **6. The "Why" (Defense for Interview)**

| **Logic** | **Why do we do this?** |
| --- | --- |
| **cv.notify_one() inside Destructor** | **Speed.** If we don't notify, the destructor hangs for 3 seconds waiting for the timeout to finish. Notify wakes it up instantly. |
| **!running && buffer.empty()** | **Data Integrity.** We don't want to kill the thread while it still has data in the buffer. We finish the work first. |
| **while(true) vs if** | **Spurious Wakeups.** The OS might wake up a thread randomly. A while loop forces it to check the condition again to be sure. |
| **unique_lock vs lock_guard** | **Flexibility.** cv.wait requires unique_lock because it needs to unlock/re-lock internally. lock_guard cannot do that. |
| **break statement** | **Life & Death.** Breaking the loop is the *only* way to kill the background thread gracefully. |

## **7. Syntax Checklist (Don't forget these!)**

1. **std::ref(variable)**: If passing a variable to a thread function without a lambda.
2. **joinable()**: Always check if(t.joinable()) before joining (good practice).
3. **mutable**: If a lambda needs to modify a captured value (rare, but good to know).
4. **this**: Pass this as the second argument when threading a member function: thread(&Class::func, this).