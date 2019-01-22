#include "EventListener.h"
#include <iostream>
#include <KVS.oculus/Lib/Screen.h>


namespace kvs
{

namespace leap
{

EventListener::EventListener( kvs::oculus::Screen* screen ):
//    Leap::Listener( m_controller ),
//    m_controller( *this ),
//    m_controller( new Leap::Controller( (Leap::Listener&)*this ) ),
    m_screen( screen )
{
//    m_controller.addListener( *this );
//    m_controller.setPolicy( Leap::Controller::POLICY_IMAGES );
}

EventListener::~EventListener()
{
//    m_controller.removeListener( *this );
}

void EventListener::onInit( const Leap::Controller& controller )
{
    std::cout << "EventListener::onInit()" << std::endl;
    this->initializeEvent();
}

void EventListener::onFrame( const Leap::Controller& controller )
{
    std::cout << "EventListener::onFrame()" << std::endl;
    this->frameEvent();
}

} // end of namespace leap

} // end of namespace kvs
