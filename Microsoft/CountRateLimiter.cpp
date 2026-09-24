#include <chrono>

using namespace std;

class RateLimiter {
private:
    unordered_map<string, queue<long>> clientRequests;
    int limit;
    long windowSeconds;
    mutex mtx;

    long now() {
        return chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }

public:
    RateLimiter(int limit, long windowSeconds) : limit(limit), windowSeconds(windowSeconds) {}
    
    bool allowRequest(string clientId){
        lock_guard<mutex> lock(mtx); 
        long currentTime = now();
        queue<long>& q = clientRequests[clientId];
        while(!q.empty() && q.front() < currentTime-windowSeconds){
            q.pop();
        }
        if(q.size() < limit){
            q.push(currentTime);
            return true;
        }
        return false;
    }
}
