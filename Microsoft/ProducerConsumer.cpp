// Problem: Implement a bounded buffer shared between producer and consumer threads.

// void produce(int item) — adds an item to the buffer. 
// If the buffer is full, blocks (no busy-waiting) until there's space.
// int consume() — removes and returns an item from the buffer. 
// If the buffer is empty, blocks until an item is available.
// Buffer capacity: fixed at construction time.
// Must be correct under multiple producer and consumer threads running concurrently — no lost items, no duplicates, no busy-waiting.
#include <semaphore>

using namespace std;

class ProducerConsumer{
private:
    queue<int> buffer;
    counting_semaphore<> emptySlots;
    counting_semaphore<> fullSlots;
    int capacity;
    mutex mtx;

public:
    ProducerConsumer(int cap) : emptySlots(cap), capacity(cap), fullSlots(0) {};
    
    void produce(int item){
        emptySlots.acquire();
        lock_guard<mutex> lock(mtx);
        buffer.push(item);
        fullSlots.release();
    }
    int consume(){
        fullSlots.acquire();
        lock_guard<mutex> lock(mtx);
        int a = buffer.front();
        buffer.pop();
        emptySlots.release();
        return a;
    }
}