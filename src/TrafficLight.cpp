#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
    std::unique_lock<std::mutex> myLock (_mutex);
    _cond.wait(myLock, [this] () { return !MessageQueue::_queue.empty(); });
    T messageReceived = std::move(MessageQueue::_queue.back());
    MessageQueue::_queue.pop_back();

    return messageReceived;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> Lock(_mutex);
    MessageQueue::_queue.clear();
    MessageQueue::_queue.emplace_back(std::move(msg));
    MessageQueue::_cond.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

TrafficLight::~TrafficLight()
{
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop
    // runs and repeatedly calls the receive function on the message queue.
    // Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        // just in case as in traffic light we wait a milisecond
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        if(myMessageQueue.receive() == TrafficLightPhase::green)
            return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::setCurrentPhase(TrafficLightPhase myPhase)
{
    this->_currentPhase = myPhase;
};

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class.
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

long TrafficLight::computeCycleDuration()
{
    // FP.2a : The cycle duration should be a random value between 4 and 6 seconds.
    srand(time(NULL)); // Seed the time 
    //srand((unsigned) time(0));
    std::random_device randomDevice;
    std::mt19937 gen(randomDevice());
    long myMax = 6000;   //miliseconds
    long myMin = 4000;   //miliseconds
    std::uniform_real_distribution<> dist(myMin,myMax);
    long cycleDuration = (long) dist(gen); // in miliseconds
    //std::cout << "Current cycle duration (ms) "<<cycleDuration<< std::endl;
    return cycleDuration;
}

void TrafficLight::togglePhase()
{
    // toggle light phases
    if (this->getCurrentPhase() == TrafficLightPhase::green)
    {
        this->setCurrentPhase(TrafficLightPhase::red);
    }
    else if (this->getCurrentPhase() == TrafficLightPhase::red)
    {
        this->setCurrentPhase(TrafficLightPhase::green);
    }
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles
    
    
    bool isNewCycle = true;
    int cycleDuration;
    
    // Get starting timepoint
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();


    while (true)
    {
        // get new cycle duration
        if (isNewCycle)
        {
            cycleDuration = computeCycleDuration();
            isNewCycle = false;
        }            

        // FP.2a : Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles.
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Get ending timepoint and duration of time
        std::chrono::time_point<std::chrono::high_resolution_clock> stopTime = std::chrono::high_resolution_clock::now();
        long durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(stopTime - startTime).count();
        
        if (durationTime >= cycleDuration)
        {
            // FP.2a : and toggles the current phase of the traffic light between red and green
            togglePhase();

            // reset starting timepoint for next cycle
            isNewCycle =  true;
            startTime = std::chrono::high_resolution_clock::now();
        }

        
        // FP.2  and sends an update method to the message queue using move semantics.
        myMessageQueue.send(std::move(_currentPhase));
    }
    
}
