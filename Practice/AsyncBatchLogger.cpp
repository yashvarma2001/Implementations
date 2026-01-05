// Question 2: The Asynchronous Batch Logger
// The Scenario: We are building a high-performance server. 
// We cannot write every log message to the disk immediately because disk I/O is slow and blocks the main thread.

// The Task: Implement a class AsyncLogger that:

// Accepts logs instantly: log(string msg) should add the message to a buffer and return immediately (Non-blocking).

// Flushes automatically: A background thread should write the logs to the console (simulating disk) 
// if EITHER of these conditions is met:

// Capacity: The buffer reaches 5 messages.
// Time: 3 seconds have passed since the last flush.
// Graceful Shutdown: When the stop() function is called (or destructor runs), 
// make sure all remaining logs in the buffer are printed before the thread dies.


#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

class AsyncLogger {
private:
    vector<string> buffer;
    mutex mtx;
    condition_variable cv;
    bool running = true;
    thread worker;

    void workerLoop() {
        while (true) {
            unique_lock<mutex> lock(mtx);
            
            cv.wait_for(lock, chrono::seconds(2), [this] {
                return buffer.size() >= 5 || !running;
            });

            if (!running && buffer.empty()) {
                break;
            }

            vector<string> logs_to_print;
            logs_to_print.swap(buffer);
            
            lock.unlock();

            if (!logs_to_print.empty()) {
                cout << "--- WRITING BATCH (" << logs_to_print.size() << ") ---" << endl;
                for (string s : logs_to_print) {
                    cout << s << endl;
                }
            }
        }
    }

public:
    AsyncLogger() {
        worker = thread(&AsyncLogger::workerLoop, this);
    }

    ~AsyncLogger() {
        {
            lock_guard<mutex> lock(mtx);
            running = false;
        }
        cv.notify_one();
        worker.join();
    }

    void log(string msg) {
        lock_guard<mutex> lock(mtx);
        buffer.push_back(msg);
        
        if (buffer.size() >= 5) {
            cv.notify_one();
        }
    }
};


int main() {
    AsyncLogger logger;

    cout << "Main: Logging 3 messages (will wait for timeout)..." << endl;
    logger.log("Log 1");
    logger.log("Log 2");
    logger.log("Log 3");

    this_thread::sleep_for(chrono::seconds(3));

    cout << "Main: Logging 10 messages (will trigger batching)..." << endl;
    for(int i = 0; i < 10; i++) {
        logger.log("Fast Log " + to_string(i));
    }

    cout << "Main: Exiting... (Destructor will handle remaining logs)" << endl;
    return 0;
}