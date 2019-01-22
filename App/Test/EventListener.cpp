#include "EventListener.h"
#include <iostream>
#include <KVS.oculus/Lib/Screen.h>


namespace kvs
{

namespace leap
{

EventListener::EventListener( kvs::oculus::Screen* screen ):
    m_screen( screen )
{
    m_controller.addListener( *this );
    m_controller.setPolicy( Leap::Controller::POLICY_IMAGES );
}

EventListener::~EventListener()
{
    m_controller.removeListener( *this );
}

void EventListener::onInit( const Leap::Controller& controller )
{
    this->initializeEvent();
}

void EventListener::onFrame( const Leap::Controller& controller )
{
    this->frameEvent();
}

void EventListener::onConnect( const Leap::Controller& controller )
{
    this->connectEvent();
}

void EventListener::onDisconnect( const Leap::Controller& controller )
{
    this->disconnectEvent();
}

void EventListener::onExit( const Leap::Controller& controller )
{
    this->exitEvent();
}

} // end of namespace leap

} // end of namespace kvs
