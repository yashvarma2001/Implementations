#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct Job {
    function<void()> task;
    steady_clock::time_point executeAt;

    bool operator>(const Job& other) const {
        return executeAt > other.executeAt;
    }
};

class Scheduler {
private:
    priority_queue<Job, vector<Job>, greater<Job>> pq; 
    mutex mtx;
    condition_variable cv; 
    bool stop = false;

public:
    void schedule(function<void()> func, int delayMs) {
        lock_guard<mutex> lock(mtx);
        
        auto now = steady_clock::now();
        Job newJob = { func, now + milliseconds(delayMs) };
        
        pq.push(newJob);
        cv.notify_one(); 
    }

    void run() {
        unique_lock<mutex> lock(mtx);

        while (!stop || !pq.empty()) {
            
            if (pq.empty()) {
                if(stop) return;
                cv.wait(lock);
            } 
            else {
                Job job = pq.top();
                auto now = steady_clock::now();

                if (now >= job.executeAt) {
                    // 1. READY TO RUN
                    pq.pop(); // Remove from queue
                    
                    // Critical: Unlock while running the task so we don't block schedule()
                    lock.unlock();
                    job.task(); 
                    lock.lock(); // Lock again for next loop iteration
                } 
                else {
                    // 2. TOO EARLY (Wait)
                    // Sleep until the job time... OR until interrupted by a new earlier job!
                    // cv.wait_until unlocks mutex, sleeps, and re-locks on wake.
                    cv.wait_until(lock, job.executeAt);
                }
            }
        }
    }

    void stopScheduler() {
        {
            lock_guard<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all(); // Wake up worker so it can exit
    }
};

// --- TEST MAIN ---
int main() {
    Scheduler scheduler;

    // 1. Start the worker thread
    thread worker(&Scheduler::run, &scheduler);

    cout << "Main: Scheduling Task A (Run in 2s)..." << endl;
    scheduler.schedule([]() { 
        cout << "Task A Executed! (Expected ~2s)" << endl; 
    }, 2000);

    cout << "Main: Scheduling Task B (Run in 10s)..." << endl;
    scheduler.schedule([]() { 
        cout << "Task B Executed! (Expected ~10s)" << endl; 
    }, 10000);

    // THE INTERRUPT TEST
    this_thread::sleep_for(seconds(1));
    cout << "Main: INTERRUPT! Scheduling Task C (Run in 3s) - Worker should wake up!" << endl;
    
    // Worker is currently sleeping for 10s (Task B). 
    // This insert should wake it up to realize Task C (3s) is earlier.
    scheduler.schedule([]() { 
        cout << "Task C Executed! (Expected ~3s - Before B!)" << endl; 
    }, 3000);

    // Let them run
    this_thread::sleep_for(seconds(12));
    
    scheduler.stopScheduler();
    worker.join();
    
    return 0;
}