#pragma once
#include <Leap.h>
#include <KVS.oculus/Lib/Screen.h>
#include <kvs/Vector3>


namespace kvs
{

namespace leap
{

class EventListener : public Leap::Listener
{
private:
    Leap::Controller m_controller;
    kvs::oculus::Screen* m_screen;

public:
    EventListener( kvs::oculus::Screen* screen );
    ~EventListener();

    virtual void initializeEvent() {}
    virtual void frameEvent() {}

protected:
    Leap::Controller& controller() { return m_controller; }
    kvs::oculus::Screen& screen() { return *m_screen; }
    kvs::Vec3 leapToWorld( const Leap::Vector& p_leap ) const;

private:
    virtual void onInit( const Leap::Controller& controller );
    virtual void onFrame( const Leap::Controller& controller );
    virtual void onConnect( const Leap::Controller& controller );
    virtual void onDisconnect( const Leap::Controller& controller );
    virtual void onExit( const Leap::Controller& controller );
};

} // end of namespace leap

} // end of namespace kvs
