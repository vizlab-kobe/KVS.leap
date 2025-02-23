#include "ControllerBase.h"
#include <iostream>
#include <KVS.oculus/Lib/Screen.h>
#include <KVS.oculus/Lib/OVR.h>
#include <kvs/Matrix44>
#include <kvs/Quaternion>


namespace kvs
{

namespace leap
{

ControllerBase::ControllerBase( kvs::oculus::Screen* screen ):
    m_screen( screen )
{
    m_input_device.addListener( *this );
    m_input_device.setPolicy( Leap::Controller::POLICY_IMAGES );
}

ControllerBase::~ControllerBase()
{
    m_input_device.removeListener( *this );
}

void ControllerBase::onInit( const Leap::Controller& controller )
{
    this->initializeEvent();
}

void ControllerBase::onFrame( const Leap::Controller& controller )
{
    this->frameEvent();
}

kvs::Vec3 ControllerBase::leapToWorld( const Leap::Vector& p_leap ) const
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
