#pragma once
#include "EventListener.h"


namespace kvs
{

namespace leap
{

class GestureEventListener : public kvs::leap::EventListener
{
public:
    GestureEventListener( kvs::oculus::Screen* screen );

    void swipeEvent();
    void circleEvent();
    void screenTapEvent();
    void keyTapEvent();

protected:
    virtual void frameEvent();
};

} // end of namespace leap

} // end of namespace kvs
