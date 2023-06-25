#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H

class ThetisSubsystem {
public:
    virtual bool begin();
    virtual void poll();

protected:

private:

};

#endif // SUBSYSTEM_H