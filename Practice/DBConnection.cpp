#include <iostream>
#include <queue>
#include <unordered_set> // NEW: To track borrowed items
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

using namespace std;

class DBConnection {
public:
    int id;
    DBConnection(int id) : id(id) {}
    void query(string q) { cout << "Conn " << id << ": " << q << endl; }
};

class ConnectionPool {
private:
    queue<DBConnection*> pool;
    // NEW: The "Ledger" of currently borrowed connections
    unordered_set<DBConnection*> leased_connections; 
    
    int maxCapacity;
    mutex mtx;
    condition_variable cv;

public:
    ConnectionPool(int maxConnections) {
        this->maxCapacity = maxConnections;
        for (int i = 0; i < maxConnections; i++) {
            pool.push(new DBConnection(i));
        }
    }

    DBConnection* getConnection(int timeout_ms) {
        unique_lock<mutex> lock(mtx);

        bool got_connection = cv.wait_for(lock, chrono::milliseconds(timeout_ms), [this] {
            return !pool.empty();
        });

        if (!got_connection) return nullptr;

        DBConnection* connection = pool.front();
        pool.pop();

        // VALIDATION STEP 1: Add to the ledger
        leased_connections.insert(connection);
        
        return connection;
    }

    void releaseConnection(DBConnection* conn) {
        lock_guard<mutex> lock(mtx);
        
        // VALIDATION STEP 2: Check the ledger
        // If this connection is NOT in our leased list, it's a fake or a double-return.
        if (leased_connections.find(conn) == leased_connections.end()) {
            cout << "ERROR: Attempted to release invalid or already returned connection!" << endl;
            return; 
        }

        // Remove from ledger
        leased_connections.erase(conn);

        // Safely return to pool
        pool.push(conn);
        cv.notify_one();
    }
};

int main() {
    ConnectionPool pool(2);

    DBConnection* validConn = pool.getConnection(100);
    DBConnection* fakeConn = new DBConnection(999);

    cout << "--- Trying to release fake connection ---" << endl;
    pool.releaseConnection(fakeConn); // Should print ERROR and do nothing

    cout << "--- Returning valid connection ---" << endl;
    pool.releaseConnection(validConn); // Should work

    cout << "--- Returning valid connection AGAIN (Double Free) ---" << endl;
    pool.releaseConnection(validConn); // Should print ERROR

    return 0;
}