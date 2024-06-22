



#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

class ThreadSafeQueue {
public:
    
    static void initializeQueue();
private:
    // Pushes an element into the queue
    void push(const int& value);
    // Pops an element from the queue
    int pop();
    std::queue<int> queue_; // The underlying queue
    std::mutex mutex_; // Mutex for protecting access to the queue
    std::condition_variable condVar_; // Condition variable for synchronization
};
