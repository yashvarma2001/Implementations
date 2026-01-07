// ### Question 4: The Distributed Lock (Redis Simulator)

// **The Scenario:**
// You are building the core locking engine for a distributed system (like Redis). 
// Multiple servers (nodes) will request a lock from your class.

// Since servers can crash, your lock must support **TTL (Time-To-Live)**. 
// If a server takes the lock but dies (crashes) before releasing it, the lock must automatically expire so others can acquire it.

// **Requirements:**

// 1. **Class Name:** `DistributedLock`
// 2. **Internal State:**
// * `owner`: Stores the ID of the node currently holding the lock (string).
// * `expiryTime`: Stores the exact timestamp when the lock expires.
// * `mutex`: Ensures thread safety for the state variables.

// 3. **Methods:**
// * `bool tryAcquire(string nodeID, int ttlSeconds)`:
// * **Success:** If the lock is free **OR** the current lock has expired: Set the new owner, calculate the new expiry time (`now + ttl`), and return `true`.
// * **Failure:** If someone else holds the lock and it hasn't expired yet: Return `false`.

// * `void release(string nodeID)`:
// * Releases the lock **ONLY IF** the `nodeID` matches the current owner. (Security check).

// 4. **Test Harness:**
// * Simulate Node A acquiring the lock and releasing it properly.
// * Simulate Node B acquiring the lock, "crashing" (sleeping without releasing), and then Node C successfully acquiring it after the TTL expires.

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;
using namespace std::chrono;

class DistributedLock{
private:
    string owner;
    time_point<steady_clock> expiryTime;
    mutex mtx;
public:
    DistributedLock(){
        this->owner = "";
    }
    bool tryAcquire(string nodeID, int ttlSeconds){
        lock_guard<mutex> lock(mtx);
        auto curr_time = steady_clock::now();
        if(owner != "" && curr_time <= expiryTime){
            return false;
        }
        owner = nodeID;
        expiryTime = curr_time + seconds(ttlSeconds);
        return true;
    }

    void release(string nodeID){
        lock_guard<mutex> lock(mtx);
        if(owner == nodeID){
            owner = "";
            return;
        }
    }
};

void serverNode(DistributedLock& l, string nodeId){
    bool success = l.tryAcquire(nodeId, 3);
    if (success) {
        cout << nodeId << ": I am working on the database..." << endl;
        this_thread::sleep_for(seconds(2)); // Simulate work
        l.release(nodeId);
    } else {
        cout << nodeId << ": Locked! I will retry later." << endl;
    }
};

int main(){
    DistributedLock newLock;
    thread tA(serverNode, ref(newLock), "Node_A");
    this_thread::sleep_for(milliseconds(100));
    tA.join();

    thread tB([&](){
        newLock.tryAcquire("Node_B", 1);
        cout<< "Node_B crashed without calling release" << endl;
    });

    tB.join();
    this_thread::sleep_for(seconds(2));

    thread tC(serverNode, ref(newLock), "Node_C"); // Should succeed because B expired
    tC.join();

    return 0;

}