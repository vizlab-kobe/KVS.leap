#include "EventListener.h"
#include <iostream>
#include <KVS.oculus/Lib/Screen.h>
#include <KVS.oculus/Lib/OVR.h>
#include <kvs/Matrix44>
#include <kvs/Quaternion>


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

kvs::Vec3 EventListener::leapToWorld( const Leap::Vector& p_leap ) const
{
    const kvs::oculus::HeadMountedDisplay& hmd = m_screen->headMountedDisplay();
    const double time = kvs::oculus::TimeInSecond();
    const ovrTrackingState state = hmd.trackingState( time );
    const ovrPosef headPose = state.HeadPose.ThePose;

    kvs::Mat4 M_ow = kvs::Mat4::Identity();
    kvs::Quaternion orientation( headPose.Orientation.x, headPose.Orientation.y, headPose.Orientation.z, headPose.Orientation.w );
    orientation.toMatrix( M_ow );

    const kvs::Mat4 M_lo(
        -1,  0,  0, 0,
         0,  0, -1, 0,
         0, -1,  0, -0.08f,
         0,  0,  0, 1 );

    const kvs::Mat4 M(
        -1, 0,  0, 0,
         0, 1,  0, 0,
         0, 0, -1, 0,
         0, 0,  0, 1 );

    const kvs::Mat4 M_lw = M_ow * M_lo;
    const kvs::Vec4 p_world = M_lw * M * kvs::Vec4( p_leap.x, p_leap.y, p_leap.z, 1 );
    return p_world.xyz();
}

} // end of namespace leap

} // end of namespace kvs
