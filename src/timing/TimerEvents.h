#ifndef TIMER_EVENTS_H
#define TIMER_EVENTS_H

#include "Arduino.h"

typedef std::function<void()> TimerEventHandler;

class TimerEvent {

public:
    TimerEvent(const char* n, unsigned long p, TimerEventHandler cbPtr, bool e);

    const char* name;
    unsigned long period;
    TimerEventHandler callbackPtr = nullptr;
    bool isEnabled;
    int index = -1;

    void execute() { if (callbackPtr != nullptr) callbackPtr(); }
    void setEventCallback(TimerEventHandler cbPtr) { callbackPtr = cbPtr; }
    void enable() { isEnabled = true; }
    void disable() { isEnabled = false; }
};

class TimerEventsClass {

public:
    TimerEventsClass();
    void add(TimerEvent* event);
    void update(TimerEvent* event);
    void tasks();

    void printTasking(Stream* out);

    int getNumberOfEvents() { return numberOfEvents; }

private:
    static const int maxNumberOfEvents = 32;
    int numberOfEvents;
    unsigned long timeouts[maxNumberOfEvents];
    TimerEvent* events[maxNumberOfEvents]; 
};

extern TimerEventsClass timerEvents;

#endif
