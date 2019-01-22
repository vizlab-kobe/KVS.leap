#include "GestureEventListener.h"

namespace kvs
{

namespace leap
{

GestureEventListener::GestureEventListener( kvs::oculus::Screen* screen ):
    kvs::leap::EventListener( screen )
{
    controller().enableGesture( Leap::Gesture::TYPE_CIRCLE );
    controller().enableGesture( Leap::Gesture::TYPE_KEY_TAP );
    controller().enableGesture( Leap::Gesture::TYPE_SCREEN_TAP );
    controller().enableGesture( Leap::Gesture::TYPE_SWIPE );
}

void GestureEventListener::swipeEvent()
{
    std::cout << "Swipe" << std::endl;
}

void GestureEventListener::circleEvent()
{
    std::cout << "Circle" << std::endl;
}

void GestureEventListener::screenTapEvent()
{
    std::cout << "Screen Tap" << std::endl;
}

void GestureEventListener::keyTapEvent()
{
    std::cout << "Key Tap" << std::endl;
}

void GestureEventListener::frameEvent()
{
    const Leap::Frame frame = controller().frame();

    const Leap::HandList hands = frame.hands();
    for ( Leap::HandList::const_iterator hand = hands.begin(); hand != hands.end(); ++hand )
    {

    }

    const Leap::GestureList gestures = frame.gestures();
    for ( int i = 0; i < gestures.count(); ++i )
    {
        const Leap::Gesture g = gestures[i];
        switch ( g.type() )
        {
        case Leap::Gesture::TYPE_SWIPE:
            this->swipeEvent();
            break;
        case Leap::Gesture::TYPE_CIRCLE:
            this->circleEvent();
            break;
        case Leap::Gesture::TYPE_SCREEN_TAP:
            this->screenTapEvent();
            break;
        case Leap::Gesture::TYPE_KEY_TAP:
            this->keyTapEvent();
            break;
        default:
            break;
        }
    }
}

} // end of namespace leap

} // end of namespace kvs
