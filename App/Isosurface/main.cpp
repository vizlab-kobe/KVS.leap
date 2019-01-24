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
#include <kvs/Message>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonObject>
#include <kvs/Isosurface>
#include <kvs/Streamline>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/HydrogenVolumeData>
#include <kvs/glut/Application>
#include <kvs/glut/Screen>
#include <Leap.h>
#include <unistd.h>
#include <kvs/Bounds>

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
    usleep(100);
    //InteractionBox
    Leap::InteractionBox iBox = frame.interactionBox();    
    
    Leap::ImageList images = frame.images();
    for(int i = 0; i < 2; i++){
      Leap::Image image = images[i];

      const unsigned char* image_buffer = image.data();
    }
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
	    m_screen.scene()->object("Iso")->show();
	        
	  }
	Leap::FingerList thumbList = hand.fingers().fingerType(Leap::Finger::TYPE_THUMB);
	Leap::Finger thumb = thumbList[0];
	Leap::FingerList indexList = hand.fingers().fingerType(Leap::Finger::TYPE_INDEX);
	Leap::Finger indexFinger = indexList[0];
	std::cout<< "finger count"<< frame.fingers().count()<<std::endl;
	if( thumb.isExtended() && indexFinger.isExtended()){
	  
	  usleep(1000);
	  
	  float distance = (thumb.tipPosition()).distanceTo(indexFinger.tipPosition()); 
	  std::cout <<"distance:"<<distance <<std::endl;
	  double d = 0.0;
	  if(distance > 100) {
	    distance = 100;
	    d = (m;
	  }
	  else if(distance <20)
	    {
	      distance = 20;
	      d = m_volume->maxValue();
	    }
	  d = m_volume->maxValue()-distance*(m_volume->maxValue()/80);
	  // std::cout <<"min_volume:"<<m_volume ->minValue()<< "max_volume"<<m_volume->maxValue() <<std::endl;
	  const double i = d;
	  const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
	  
	  const kvs::TransferFunction t( 256 );
	  kvs::PolygonObject* object = new kvs::Isosurface( m_volume, i, n, d, t );
	   
	  
	  object->setName("Iso");
	  m_screen.scene()->replaceObject("Iso",object);
	  
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
    kvs::glut::Application app( argc, argv );
    
    /* Read volume data from the specified data file. If the data file is not
     * specified, scalar hydrogen volume data is created by using
     * kvs::HydrogenVolumeData class.
     */
    kvs::StructuredVolumeObject* volume = NULL;
    if ( argc > 1 ) {
      volume = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );
      volume = new kvs::StructuredVectorToScalar(volume);
    }
    else            volume = new kvs::HydrogenVolumeData( kvs::Vector3ui( 64, 64, 64 ) );

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
 
    // Screen.
    kvs::glut::Screen screen( &app );
    screen.registerObject( object );
    screen.registerObject(object ,new kvs::Bounds());
    //screen.setGeometry( 0, 0, 512, 512 );
    screen.setTitle( "kvs::Isosurface" );
    screen.show();

    SampleListener listener( screen, volume );
    Leap::Controller controller;
    controller.addListener( listener );
    controller.setPolicy(Leap::Controller::POLICY_IMAGES);
 controller.setPolicy(Leap::Controller::PolicyFlag::POLICY_OPTIMIZE_HMD);

    //    controller.removeListener(listener);

    return( app.run() );
}
