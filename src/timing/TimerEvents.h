#ifndef TIMER_EVENTS_H
#define TIMER_EVENTS_H

#include "Arduino.h"

typedef std::function<void()> TimerEventHandler;

class TimerEvent {

public:
    TimerEvent(unsigned long p, TimerEventHandler cbPtr, bool e);

    unsigned long period;
    TimerEventHandler callbackPtr = nullptr;
    bool isEnabled;

    void execute() { if (callbackPtr != nullptr) callbackPtr(); }
    void setEventCallback(TimerEventHandler cbPtr) { callbackPtr = cbPtr; }
    void enable() { isEnabled = true; }
    void disable() { isEnabled = false; }
};

class TimerEventsClass {

public:
    TimerEventsClass();
    void add(TimerEvent* event);
    void tasks();

private:
    static const int maxNumberOfEvents = 32;
    int numberOfEvents;
    unsigned long timeouts[maxNumberOfEvents];
    TimerEvent* events[maxNumberOfEvents]; 
};

extern TimerEventsClass timerEvents;

#endif
