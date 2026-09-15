
class DBConnection{
private:
    struct Connection{
        string ID;
        Connection(string id){
            ID = id;
        };
    };
    vector<Connection*> availableConnections;
    unordered_set<Connection*> connectionsInUse;
    mutex mtx;
    condition_variable cv;
public:
    DBConnection(){
        for(int i=0;i<3;i++){
            Connection* c = new Connection(i+'a');
            availableConnections.push_back(c);
        }
    }
    
    Connection* acquire(){
        unique_lock<mutex> lock(mtx);
        if(availableConnections.empty()){
            cv.wait(lock, [this](){ return !availableConnections.empty(); });
        }
        Connection* c = availableConnections.front();
        connectionsInUse.insert(c);
        availableConnections.erase(availableConnections.begin());
        return c;
    }

    void release(Connection* conn){
        lock_guard<mutex> lock(mtx);
        if(!connectionsInUse.count(conn)){
            return;
        }
        connectionsInUse.erase(conn);
        availableConnection.push_back(conn);
        cv.notify_one();
    }
}
