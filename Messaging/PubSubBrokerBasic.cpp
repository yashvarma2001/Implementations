#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// 1. Message Class
class Message {
private:
    string text;
public:
    Message(string t){
        this->text = t;
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

public:
    void subscribe(string topic, Subscriber* sub) {
        subscribers[topic].push_back(sub);
    }

    void publish(string topic, Message* msg) {
        if (subscribers.find(topic) != subscribers.end()) {
            for (Subscriber* sub : subscribers[topic]) {
                sub->receive(msg->text);
            }
        }
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