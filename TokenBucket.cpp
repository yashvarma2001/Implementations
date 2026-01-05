#include <iostream>
#include <mutex> // Lock library
#include <chrono> // handles precise time
#include <algorithm> // math helper library

using namespace std;

class TokenBucket {
private:
    long capacity;
    double refillRate;
    double currentTokens;
    
    chrono::steady_clock::time_point lastRefillTime; //time_point: Represents a specific instant in time (a timestamp).
    mutex mtx; // a Mutex guards the Critical Section

public:
    TokenBucket(long cap, double rate) {
        this->capacity = cap;
        this->refillRate = rate;
        this->currentTokens = cap;
        this->lastRefillTime = chrono::steady_clock::now();
    }

    bool allowRequest(int tokensNeeded) {
        // A Mutex locks a Section of Code.
        lock_guard<mutex> lock(mtx); // acquires mutex lock on execution of this line
        // automatically releases at end of function.

        auto now = chrono::steady_clock::now();
        chrono::duration<double> elapsed = now - lastRefillTime; // duration: Represents a time interval (the difference between two time_points).
        double secondsPassed = elapsed.count(); // .count(): A method that returns the numerical value of the duration (in seconds, because we cast it to duration<double>)

        double tokensToAdd = secondsPassed * refillRate;

        if (tokensToAdd > 0) {
            currentTokens = min((double)capacity, currentTokens + tokensToAdd);
            lastRefillTime = now;
        }

        if (currentTokens >= tokensNeeded) {
            currentTokens -= tokensNeeded;
            return true;
        }
        return false;
    }
};