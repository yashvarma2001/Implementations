#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex> // Added for you

using namespace std;

class Message {
private:
    string text;
public:
    Message(string t){
        this->text = t;
    }
    string getText() { 
        return this->text; 
    }
};

class Subscriber {
private:
    string name;
public:
    Subscriber(string n){
        this->name = n;
    }

    void receive(string msg_text) {
        cout << "[" << name << "] received: " << msg_text << endl;
    }
};

class Broker {
private:
    unordered_map<string, vector<Subscriber*>> subscribers;
    
    // TODO: Use this mutex to protect the map
    mutex mtx; 

public:
    void subscribe(string topic, Subscriber* sub) {
        // TODO: Implement thread-safe subscribe
        // 1. Lock
        // 2. Add to map
    }

    void publish(string topic, Message* msg) {
        // TODO: Implement thread-safe publish
        // 1. Lock
        // 2. Copy list (to avoid blocking)
        // 3. Unlock
        // 4. Iterate and call receive()
    }
};

int main() {
    Broker myBroker;
    Subscriber sub1("Alice");
    
    myBroker.subscribe("News", &sub1);
    
    Message msg("Hello World");
    myBroker.publish("News", &msg);
    
    return 0;
}