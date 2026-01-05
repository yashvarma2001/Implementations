/*The Prompt: "The Connection Pool"
Interviewer: "At Retell, we own a limited set of 10 Twilio Phone Numbers that our AI agents use to make outbound calls.

I want you to design a class called PhoneNumberPool.

Initialize it with a list of available phone numbers (Strings).

acquire(): An agent calls this to get a phone number.

If a number is free, remove it from the pool and return it.

If no numbers are free, the agent must wait (block) until one becomes available.

release(String number): When the call is over, the agent returns the number to the pool so someone else can use it.

Constraint: This must be thread-safe. We have 50 agents trying to make calls simultaneously." */

#include <iostream>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unordered_set>

using namespace std;

class PhoneNumberPool {
private:
    queue<string> q;               
    unordered_set<string> inPool;
    int maxCapacity;
    
    mutex mtx;
    condition_variable cv;

public:
    PhoneNumberPool(const vector<string>& initialNumbers) {
        this->maxCapacity = initialNumbers.size();
        
        for (const string& num : initialNumbers) {
            q.push(num);
            inPool.insert(num);
        }
    }

    string acquire() {
        unique_lock<mutex> lock(mtx);

        while (q.empty()) {
            cv.wait(lock);
        }

        string number = q.front();
        q.pop();
        
        inPool.erase(number);

        return number;
    }

    void release(string number) {
        lock_guard<mutex> lock(mtx);

        if (q.size() >= maxCapacity) {
            return;
        }

        if (inPool.find(number)!=inPool.end()) {
            return;
        }

        q.push(number);
        inPool.insert(number);
        cv.notify_one(); 
    }
};