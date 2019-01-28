#pragma once
#include "ControllerBase.h"


namespace kvs
{

namespace leap
{

class GestureController : public kvs::leap::ControllerBase
{
private:
    bool m_is_grabbed;
    float m_palm_distance;

public:
    GestureController( kvs::oculus::Screen* screen );

    void swipeEvent() {}
    void circleEvent() {}
    void screenTapEvent() {}
    void keyTapEvent() {}

protected:
    virtual void frameEvent();
};

} // end of namespace leap

} // end of namespace kvs
