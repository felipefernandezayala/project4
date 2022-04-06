#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


template <class T>
class MessageQueue
{
public:
    // FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
    // Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
    void send(T && myMessage);
    T receive();

private:
    // Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
    // Also, there should be an std::condition_variable as well as an std::mutex as private members. 
    std::deque<T> _queue;
    std::mutex _mutex;
    std::condition_variable _cond;
};


enum class TrafficLightPhase {red, green}; //FP.1 :where TrafficLightPhase is an enum that can be either „red“ or „green“.

// FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
class TrafficLight : public TrafficObject
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase(); //FP.1  as well as „TrafficLightPhase getCurrentPhase()“,  
    void setCurrentPhase( TrafficLightPhase myPhase);

    // typical behaviour methods
    void waitForGreen(); //FP.1 : The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
    void simulate();
   


private:
    // typical behaviour methods

    // FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    // and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    // send in conjunction with move semantics.
    MessageQueue<TrafficLightPhase> myMessageQueue;

    std::condition_variable _condition;
    std::mutex _mutex;
    
    void cycleThroughPhases(); //FP.1 :Also, add the private method „void cycleThroughPhases()“. 
    TrafficLightPhase _currentPhase; //FP.1 : Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

    // auxiliary functions for cycle through phases
    long computeCycleDuration();
    void togglePhase();
};

#endif