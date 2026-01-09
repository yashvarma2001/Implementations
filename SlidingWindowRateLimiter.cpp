// Question 3: The Sliding Window Rate Limiter
// The Task: Implement a class SlidingWindow that tracks request timestamps.

// Rule: Allow a max of N requests in a T second window.
// Logic: When a request comes in:
// Clean: Remove all timestamps from the history that are older than T seconds.
// Count: If the number of remaining timestamps is < N, allow the request and save the current timestamp.
// Reject: Otherwise, return false.

// Constraints:
// Use std::queue to store timestamps.
// Use std::mutex for thread safety.

#include <iostream>
#include <chrono>
#include <queue>
#include <mutex>
#include <thread>

using namespace std;
using namespace std::chrono;

class SlidingWindow{

private:
    int maxRequests;
    seconds windowSizeSeconds;
    queue<time_point<steady_clock>> history;
    mutex mtx;

public:
    SlidingWindow(int maxRequests, int windowSeconds){
        this->maxRequests = maxRequests;
        this->windowSizeSeconds = seconds(windowSeconds);
    }
    bool allowRequest(){
        lock_guard<mutex> lock(mtx);
        auto now = steady_clock::now();

        while(!history.empty() && (now - history.front() > windowSizeSeconds)){
            history.pop();
        }
        
        if(history.size() >= maxRequests){
            return false;
        }

        history.push(now);
        return true;
    }
};

int main(){
    SlidingWindow window(2, 5);
    vector<thread> threads;
    for(int i =0;i<5;i++){
        threads.push_back( thread( [&,i]() {
            bool allowed = window.allowRequest();
            if (allowed) {
                 cout << "Thread " << i << ": Allowed!" << endl;
            } else {
                 cout << "Thread " << i << ": REJECTED." << endl;
            }
            this_thread::sleep_for(milliseconds(100));
        }));
    }
    for(auto& t: threads){
        if(t.joinable()) t.join();
    }

    return 0;
}