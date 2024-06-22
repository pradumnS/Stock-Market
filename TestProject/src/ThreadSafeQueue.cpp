#include "../include/ThreadSafeQueue.hpp"
    // Pushes an element into the queue
void ThreadSafeQueue::push(const int& value) {
    std::unique_lock<std::mutex> lock(mutex_); // Acquire lock
    queue_.push(value); // Push value into the queue
    lock.unlock(); // Unlock the mutex
    condVar_.notify_one(); // Notify waiting threads that an element is available
}

// Pops an element from the queue
int ThreadSafeQueue::pop() {
    std::unique_lock<std::mutex> lock(mutex_); // Acquire lock
    // Wait until the queue is not empty
    condVar_.wait(lock, [this] { return !queue_.empty(); }); 
    int value = queue_.front(); // Get the front element
    queue_.pop(); // Remove the front element
    return value; // Return the extracted value
}
void ThreadSafeQueue::initializeQueue()
{
    ThreadSafeQueue queue_;

    // Producer thread
    std::thread producer([&]() {
        for (int i = 10; i < 100; ++i) {
            if(i%2 && i%3 && i%5 && i%7)
            {
            std::this_thread::sleep_for(std::chrono::microseconds(50)); // Simulate some work
            queue_.push(i);
            //std::cout << "Produced: " << i << std::endl;
            }
        }
    });

    // Consumer thread
    std::thread consumer([&]() {
        for (int i = 10; i < 100; ++i) {
            if(i%2 && i%3 && i%5 && i%7)
            {
            int value = queue_.pop();
            //std::cout << "Consumed: " << value << std::endl;
            }
        }
    });

    producer.join();
    consumer.join();
}

