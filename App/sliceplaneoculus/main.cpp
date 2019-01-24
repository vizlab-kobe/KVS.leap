/*****************************************************************************/
/**
 *  @file   main.cpp
 *  @brief  Example program for kvs::Isosurface class.
 *  @author Naohisa Sakamoto
 */
/*----------------------------------------------------------------------------
 *
 *  Copyright (c) Visualization Laboratory, Kyoto University.
 *  All rights reserved.
 *  See http://www.viz.media.kyoto-u.ac.jp/kvs/copyright/ for details.
 *
 *  $Id: main.cpp 602 2010-08-19 02:43:34Z naohisa.sakamoto $
 */
/*****************************************************************************/
#include <kvs/OpenGL>
#include <Lib/Oculus.h>
#include <Lib/HeadMountedDisplay.h>
#include <Lib/Application.h>
#include <Lib/Screen.h>
#include <iostream>

#include <kvs/Message>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonObject>
#include <kvs/Isosurface>
#include <kvs/Streamline>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/SlicePlane>
#include <kvs/HydrogenVolumeData>
#include <kvs/TornadoVolumeData>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <Leap.h>
#include <unistd.h>
#include <kvs/Bounds>
#include <kvs/StylizedLineRenderer>

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/*
void segfault_sigaction(int signal, siginfo_t *si, void *arg)
{
    printf("Caught segfault at address %p\n", si->si_addr);
    //exit(0);
}*/

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
class SampleListener : public Leap::Listener
{
private:

  kvs::glut::Screen& m_screen;
  kvs::StructuredVolumeObject* m_volume;


public:

  SampleListener( kvs::glut::Screen& screen, kvs::StructuredVolumeObject* volume ) : m_screen( screen ), m_volume( volume ) {}


  void onInit( const Leap::Controller& controller )
  {
    std::cout << "Init" << std::endl;
  }

  void  onConnect(const Leap::Controller& controller)
  {
    std::cout << "Connected" << std::endl;

  }
  void onFrame( const Leap::Controller& controller )
  {
    const Leap::Frame frame = controller.frame();
    std::cout << "Frame id:" << frame.id() << std::endl;
    
    //InteractionBox
    Leap::InteractionBox iBox = frame.interactionBox();    
    
    Leap::HandList hands = frame.hands();
    for ( Leap::HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl )
      {
	std::cout << *hl << std::endl;
	int count = frame.hands().count();
	std::cout << "count" << count << std::endl;
	const Leap::Hand hand = *hl;
	
	if ( hand.isLeft() )
	  {
	    std::cout << "Left" << std::endl;
	    //m_screen.scene()->object("Iso")->hide();
	  }
	else if ( hand.isRight() )
	  {
	    std::cout << "Right" << std::endl;
	    //m_screen.scene()->object("Iso")->show();
	        
	  }
	
	std::vector<kvs::Real32> v;
	
	Leap::FingerList thumbList = hand.fingers().fingerType(Leap::Finger::TYPE_THUMB);
	Leap::Finger thumb = thumbList[0];
	Leap::FingerList indexList = hand.fingers().fingerType(Leap::Finger::TYPE_INDEX);
	Leap::Finger indexFinger = indexList[0];
	std::cout<< "finger count"<< frame.fingers().count()<<std::endl;
	if( indexFinger.isExtended() && thumb.isExtended()){
	  
	
	std::cout << "maxObjectCoord()" <<m_volume->maxObjectCoord().x()<<std::endl;
  //std::cout << "test1" << std::endl;
	
	//usleep(100);
	   
	  usleep(1000);
    //std::cout << "test2" << std::endl;
    //usleep(100);
	  Leap::InteractionBox iBox = frame.interactionBox();
    //std::cout << "test3" << std::endl;
    //usleep(100);
	  Leap::Vector vector = iBox.normalizePoint(hand.palmPosition());
    //std::cout << "test4" << std::endl;
    //usleep(100);
	  Leap::Vector nNormal = hand.palmNormal();
    //std::cout << "test5" << std::endl;
    //usleep(100);
	  vector = vector * m_volume->maxObjectCoord().x();
    //std::cout << "test6" << std::endl;
    //usleep(100);
	  const kvs::Vector3f p( vector.x, vector.y, vector.z);
    //std::cout << "test7" << std::endl;
    //usleep(100);
	  const kvs::Vector3f  n( nNormal.x, nNormal.y, nNormal.z );
    //std::cout << "test8" << std::endl;
//usleep(100);


        std::cout<<"m_volume = "<< m_volume<<" / p = "<<p<< " / n = "<<n<<std::endl;
//usleep(100);

	  const kvs::TransferFunction t( 256 );
	  kvs::PolygonObject* object = new kvs::SlicePlane( m_volume,p , n, t );
	  object->setName("Slice");
	  
	  if ( m_screen.scene()->hasObject( "Slice") )
	    {
	      m_screen.scene()->replaceObject("Slice",object );
	      
	    }
	  else
	    {
	      //m_screen.registerObject( object );  
	       m_screen.registerObject( object);
	      m_screen.registerObject( object ,new kvs::Bounds());
	    }
	}	
	
      }
    
    m_screen.redraw();
    
  }  
};
/*===========================================================================*/
/**
 *  @brief  Main function.
 *  @param  argc [i] argument counter
 *  @param  argv [i] argument values
 */
/*===========================================================================*/
int main( int argc, char** argv )
{
    /*
    struct sigaction sa;
    
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_sigaction;
    sa.sa_flags   = SA_SIGINFO;
    
    sigaction(SIGSEGV, &sa, NULL);*/
    
    //kvs::glut::Application app( argc, argv );
    kvs::oculus::Application app( argc, argv );

    /* Read volume data from the specified data file. If the data file is not
     * specified, scalar hydrogen volume data is created by using
     * kvs::HydrogenVolumeData class.
     */
    kvs::StructuredVolumeObject* volume = NULL;
    if ( argc > 1 ) {
      volume = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );
      
    }
    else   
      {  volume = new kvs::TornadoVolumeData( kvs::Vector3ui( 32, 32, 32 ) );
	volume = new kvs::StructuredVectorToScalar(volume);
      }
    if ( !volume )
      {
        kvsMessageError( "Cannot create a structured volume object." );
        return( false );
      }

    /* Extract surfaces by using the Isosurface class.
     *    i: isolevel
     *    n: NormalType (PolygonNormal/VertexNormal)
     *    d: check flag whether the duplicate vertices are deleted (false) or not
     *    t: transfer function
     */
    volume->updateMinMaxValues();
    /*
    const double i = (volume->minValue()+volume->maxValue()) ;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( volume, i, n, d, t );
    if ( !object )
    {
        kvsMessageError( "Cannot create a polygon object." );
        delete volume;
        return( false );
    }
    object->setName("Iso");
    //delete volume;
    */
    // Screen.
    //kvs::glut::Screen screen( &app );
    kvs::oculus::Screen screen( &app );
    //screen.registerObject( object );
    screen.registerObject(volume, new kvs::Bounds());
    //screen.setGeometry( 0, 0, 512, 512 );
    screen.setTitle( "kvs::Streamline" );
    screen.show();

    SampleListener listener( screen, volume );
    Leap::Controller controller;
    controller.addListener( listener );
    controller.setPolicy(Leap::Controller::POLICY_IMAGES);
    controller.setPolicy(Leap::Controller::PolicyFlag::POLICY_OPTIMIZE_HMD);

    //    controller.removeListener(listener);

    return( app.run() );
}
