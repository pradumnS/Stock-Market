#include <iostream>

#include "../include/ThreadSafeQueue.hpp"
#include "../include/readNSE.hpp"

int main()
{
    //ThreadSafeQueue::initializeQueue();
    ReadOptionData::InitialiseReadOptionData();
    std::cout<< "main func is called"<<std::endl;
    return 0;
}