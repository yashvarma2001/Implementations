#include <chrono>

using namespace std;
class Cache{
private:
    struct KeyEntry {
        string value;
        long expiryTime;
        KeyEntry(string v, int t){
            value = v;
            expiryTime = t;
        };
    };
    unordered_map<string, KeyEntry> cache;
    mutex mtx;
    long now() {
        return chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()
        ).count();
    }
public:
    void put(string key, string value, int ttl){
        lock_guard<mutex> lock(mtx);
        auto currTime = now();
        KeyEntry entry (value, currTime + ttl);
        cache[key] = entry;
    }

    optional<string> get(string key) {
    lock_guard<mutex> lock(mtx);
    auto it = cache.find(key);
    if (it == cache.end()) {
        return nullopt;
    }
    auto currTime = now();
    if (currTime > it->second.expiryTime) {
        cache.erase(key);
        return nullopt;
    }
    return it->second.value;
}

}