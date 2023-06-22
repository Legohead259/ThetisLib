#include "TimerEvents.h"

TimerEvent::TimerEvent(unsigned long p, TimerEventHandler cbPtr, bool e) {
    period = p;
    callbackPtr = cbPtr;
    isEnabled = e;
}

TimerEventsClass::TimerEventsClass() {
    numberOfEvents = 0;
}

void TimerEventsClass::add(TimerEvent* event) {
    if (numberOfEvents >= maxNumberOfEvents) {
        return;
    }
    events[numberOfEvents] = event;
    timeouts[numberOfEvents] = millis();
    numberOfEvents++;
}

void TimerEventsClass::tasks() {
    const unsigned long time = millis();
    for (int i=0; i<numberOfEvents; i++) {
        if (!events[i]->isEnabled) continue; // Check if the event is enabled, skip if not

        if (time >= timeouts[i]) {
            timeouts[i] = time + events[i]->period;
            events[i]->execute();
        }
    }
}

TimerEventsClass timerEvents;
