#include "GestureEventListener.h"
#include <kvs/Coordinate>


namespace kvs
{

namespace leap
{

GestureEventListener::GestureEventListener( kvs::oculus::Screen* screen ):
    kvs::leap::EventListener( screen ),
    m_is_grabbed( false ),
    m_palm_distance( 0.0f )
{
    inputDevice().enableGesture( Leap::Gesture::TYPE_CIRCLE );
    inputDevice().enableGesture( Leap::Gesture::TYPE_KEY_TAP );
    inputDevice().enableGesture( Leap::Gesture::TYPE_SCREEN_TAP );
    inputDevice().enableGesture( Leap::Gesture::TYPE_SWIPE );
    screen->scene()->mouse()->disableAutoUpdating();
}

void GestureEventListener::frameEvent()
{
    const Leap::Frame frame = inputDevice().frame();

    const Leap::HandList hands = frame.hands();
    if ( hands.count() == 0 ) { m_is_grabbed = false; }
    else if ( hands.count() == 1 )
    {
        const Leap::Hand& hand = hands[0];
        const Leap::Vector p = hand.palmPosition();
        const kvs::Vec2i m( kvs::Vec2( p.x, -p.z ) * 5.0f );
        if ( hand.grabStrength() > 0.5 )
        {
            if ( m_is_grabbed )
            {
                screen().scene()->mouseMoveFunction( m.x(), m.y() );
            }
            else
            {
                m_is_grabbed = true;
                kvs::Mouse::OperationMode mode = hand.isRight() ? kvs::Mouse::Rotation : kvs::Mouse::Translation;
                screen().scene()->mousePressFunction( m.x(), m.y(), mode );
            }
        }
        else
        {
            m_is_grabbed = false;
            screen().scene()->mouseReleaseFunction( m.x(), m.y() );
        }
    }
    else if ( hands.count() == 2 )
    {
        const Leap::Hand& hand0 = hands[0];
        const Leap::Hand& hand1 = hands[1];
        const Leap::Vector p0 = hand0.palmPosition();
        const Leap::Vector p1 = hand1.palmPosition();
        if ( hand0.grabStrength() > 0.5 && hand1.grabStrength() > 0.5 )
        {
            if ( m_is_grabbed )
            {
                const float palm_distance = p0.distanceTo( p1 );
                const int d = static_cast<int>( palm_distance - m_palm_distance );
                screen().scene()->wheelFunction( d * 20 );
                screen().scene()->updateXform();
                m_palm_distance = palm_distance;
            }
            else
            {
                m_is_grabbed = true;
                m_palm_distance = p0.distanceTo( p1 );
            }
        }
        else
        {
            m_is_grabbed = false;
            m_palm_distance = 0.0f;
        }
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
