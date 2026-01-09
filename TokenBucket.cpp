#include <iostream>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

class TokenBucket {
private:
    long capacity;
    double refillRate;
    double currentTokens;
    
    steady_clock::time_point lastRefillTime;
    mutex mtx;

public:
    TokenBucket(long cap, double rate) {
        this->capacity = cap;
        this->refillRate = rate;
        this->currentTokens = cap;
        this->lastRefillTime = steady_clock::now();
    }

    bool allowRequest(int tokensNeeded) {
        lock_guard<mutex> lock(mtx);

        auto now = steady_clock::now();
        duration<double> elapsed = now - lastRefillTime;
        double secondsPassed = elapsed.count();

        double tokensToAdd = secondsPassed * refillRate;

        if (tokensToAdd > 0) {
            currentTokens = min((double)capacity, currentTokens + tokensToAdd);
            lastRefillTime = now;
        }

        if (currentTokens >= tokensNeeded) {
            currentTokens -= tokensNeeded;
            cout << "Request Allowed!" << endl; // Added print to see output
            return true;
        }
        cout << "Request Denied!" << endl; // Added print to see output
        return false;
    }
};

int main() {
    TokenBucket limiter(10, 1);
    vector<thread> threads;

    for(int i = 0; i < 5; i++){
        threads.push_back(thread([&, i]() {
            bool allowed = limiter.allowRequest(1);
            if(!allowed){
                this_thread::sleep_for(seconds(2));
                limiter.allowRequest(1);
            }
        }));
        this_thread::sleep_for(milliseconds(100));
    }

    for(auto& t : threads) {
        if(t.joinable()) t.join();
    }

    return 0;
}