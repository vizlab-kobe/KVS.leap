#pragma once
#include <Leap.h>
#include <KVS.oculus/Lib/Screen.h>


namespace kvs
{

namespace leap
{

class EventListener : public Leap::Listener
{
private:
//    Leap::Controller m_controller;
    kvs::oculus::Screen* m_screen;

public:
    EventListener( kvs::oculus::Screen* screen );
    ~EventListener();

    virtual void initializeEvent() {}
    virtual void frameEvent() {}
protected:
//    const Leap::Controller& controller() const { return m_controller; }

//private:
public:
    virtual void onInit( const Leap::Controller& controller );
    virtual void onFrame( const Leap::Controller& controller );
};

} // end of namespace leap

} // end of namespace kvs
