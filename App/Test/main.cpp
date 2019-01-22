#include <kvs/HydrogenVolumeData>
#include <kvs/StructuredVolumeObject>
#include <kvs/PolygonObject>
#include <kvs/Isosurface>
#include <kvs/Bounds>
#include <KVS.oculus/Lib/Oculus.h>
#include <KVS.oculus/Lib/HeadMountedDisplay.h>
#include <KVS.oculus/Lib/Application.h>
#include <KVS.oculus/Lib/Screen.h>

#include "EventListener.h"
#include "GestureEventListener.h"


int main( int argc, char** argv )
{
    kvs::oculus::Application app( argc, argv );
    kvs::oculus::Screen screen( &app );

    kvs::Indent indent( 4 );
    std::cout << "SDK Info." << std::endl;
    std::cout << indent << "Description: " << kvs::oculus::Description() << std::endl;
    std::cout << indent << "Version: " << kvs::oculus::Version() << std::endl;
    screen.headMountedDisplay().print( std::cout << "HMD Info." << std::endl, indent );

    kvs::StructuredVolumeObject* volume = new kvs::HydrogenVolumeData( kvs::Vec3ui( 64, 64, 64 ) );
    const double i = kvs::Math::Mix( volume->minValue(), volume->maxValue(), 0.2 );
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( volume, i, n, d, t );
    delete volume;

    screen.registerObject( object, new kvs::Bounds() );
    screen.registerObject( object );
    screen.show();

//    kvs::leap::EventListener event( &screen );
    kvs::leap::GestureEventListener event( &screen );

    return app.run();
/*
    std::cout << "Description: " << kvs::oculus::Description() << std::endl;
    std::cout << "Version: " << kvs::oculus::Version() << std::endl;

    kvs::oculus::Application app( argc, argv );
    app.run();

    kvs::oculus::HeadMountedDisplay hmd;
    hmd.createDebug( ovrHmd_DK2 );

    std::cout << "Product Name: " << hmd.productName() << std::endl;
    std::cout << "Manufacturer: " << hmd.manufacturer() << std::endl;
    std::cout << "Vendor ID: " << hmd.vendorId() << std::endl;
    std::cout << "Product ID: " << hmd.productId() << std::endl;
    std::cout << "Resolution: " << kvs::oculus::ToVec2i( hmd.resolution() ) << std::endl;
    std::cout << "Window Position: " << kvs::oculus::ToVec2i( hmd.windowPosition() ) << std::endl;

    hmd.destroy();

    app.quit();

    return 0;
*/
}





#if 0
#include <kvs/OpenGL>
#include <Lib/Oculus.h>
#include <Lib/HeadMountedDisplay.h>
#include <Lib/Application.h>
#include <Lib/Screen.h>
#include <iostream>
#include <kvs/Coordinate>
#include <chrono> 

#include <kvs/Message>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonObject>
#include <kvs/PolygonRenderer>
#include <kvs/Isosurface>
#include <kvs/SlicePlane>
#include <kvs/HydrogenVolumeData>
//#include <kvs/glut/Application>
//#include <kvs/glut/Screen>
#include <kvs/Streamline>
#include <Leap.h>
#include <unistd.h>
#include <kvs/Bounds>
#include <kvs/StylizedLineRenderer>

void Scale( kvs::StructuredVolumeObject* volume, float scale )
{
  kvs::ValueArray<float> values = volume->values().asValueArray<float>();
  size_t nvalues = values.size();
  for ( size_t i = 0; i < nvalues; i++ )
    {
      values[i] *= scale;
    }

  volume->setValues( values );
  volume->updateMinMaxValues();
}


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
  
  /*  static double GetTime() {
    const std::chrono::time_point<T> end = T::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - epoch()).count() * 1e-9;
  }
  */
    kvs::Vec3 leap_to_World(Leap::Vector leapPoint)
    {
        
        kvs::Vec3 v = toVec3(leapPoint);
        
        /*
         v += kvs::Vec3(-3.0, -3.0, -3.0 );
         kvs::Vec4 v_t1 = kvs::Vec4(v,1);
         v_t1 = v_t1 * kvs::Mat4(6, 0, 0, 0,
         0, 6, 0, 0,
         0, 0, 6, 0,
         0, 0, 0, 1
         );
         v = v_t1.xyz();
         */
        kvs::oculus::HeadMountedDisplay hmd;
        //std::cout <<"hmd  "<<hmd.productName()<<std::endl;
        //  ovrHmd  m_handler = hmd.handler();
        
        double time = kvs::oculus::TimeInSecond();
        ovrTrackingState state = hmd.trackingState( time );
        ovrPosef headPose = state.HeadPose.ThePose;
        
        kvs::Mat4 M_ow = kvs::Mat4::Identity();
        kvs::Quaternion orientation(headPose.Orientation.x, headPose.Orientation.y, headPose.Orientation.z, headPose.Orientation.w);
        orientation.toMatrix(M_ow);
        kvs::Mat4 M_lo(-1, 0, 0, 0,
                       0, 0, -1, 0,
                       0, -1, 0, -0.08,
                       0, 0, 0, 1
                       );
        
        
        kvs::Mat4 M(-1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, -1, 0,
                    0, 0, 0, 1
                    );
        /*
         kvs::Mat4 M(0.01, 0, 0, 0,
         0, 0.01, 0, 0,
         0, 0, 0.01, 0,
         0, 0, 0, 1
         );
         */
        
        kvs::Mat4 M_lw = M_ow* M_lo;
        kvs::Vec4 v_t = kvs::Vec4(v,1);
        v_t = M_lw* M* v_t;
        v = v_t.xyz();
        return v;
        
    }
  kvs::Vec3 leap_to_World(Leap::Vector leapPoint, Leap::InteractionBox iBox ) 
  {
    
    std::cout<<leapPoint.x<<" "<<leapPoint.y<<" "<<leapPoint.z<<std::endl;
    kvs::Vec3 v = toVec3(leapPoint);
    std::cout<<"before"<<std::endl;
    v.print();
    Leap::Vector normalized  = iBox.normalizePoint(leapPoint);
    std::cout<<"after"<<std::endl;
    std::cout<<normalized.x<<" "<<normalized.y<<" "<<normalized.z<<std::endl;
    v = toVec3(normalized);
    v += ( m_volume->maxObjectCoord() + m_volume->minObjectCoord() ) * 0.5f ;
    kvs::Vec4 v_t1 = kvs::Vec4(v,1)*31.5f;
        
    v = v_t1.xyz();
   
    kvs::oculus::HeadMountedDisplay hmd;
    //std::cout <<"hmd  "<<hmd.productName()<<std::endl;
    //  ovrHmd  m_handler = hmd.handler();
    
    double time = kvs::oculus::TimeInSecond();
    ovrTrackingState state = hmd.trackingState( time );
    ovrPosef headPose = state.HeadPose.ThePose;
    
    kvs::Mat4 M_ow = kvs::Mat4::Identity();
    kvs::Quaternion orientation(headPose.Orientation.x, headPose.Orientation.y, headPose.Orientation.z, headPose.Orientation.w);
    orientation.toMatrix(M_ow);
    kvs::Mat4 M_lo(-1, 0, 0, 0,
		   0, 0, -1, 0,
		   0, -1, 0, -0.08,
		   0, 0, 0, 1
		   );
    
    
    kvs::Mat4 M(-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1
		);
		/*
      kvs::Mat4 M(0.01, 0, 0, 0,
      0, 0.01, 0, 0,
      0, 0, 0.01, 0,
      0, 0, 0, 1
      );
		*/
    
    kvs::Mat4 M_lw = M_ow* M_lo;
    kvs::Vec4 v_t = kvs::Vec4(v,1);
    v_t = M_lw* M* v_t;
    v = v_t.xyz();
    return v;
    
  }
  kvs::Vec3 toVec3(Leap::Vector& v)
  {
    return kvs::Vec3(v.x, v.y, v.z);
  }
  void onFrame( const Leap::Controller& controller )
  {
    const Leap::Frame frame = controller.frame();
    std::cout << "Frame id:" << frame.id() << std::endl;
    
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
	     m_screen.scene()->object("Iso")->hide();
	  }
	else if ( hand.isRight() )
	  {
	    std::cout << "Right" << std::endl;
	    m_screen.scene()->object("Iso")->show();
	    
	  }
	bool slice_plane_mode = true;
	if( slice_plane_mode ){

	  usleep(1000);
	  
	  Leap::Vector pNormal = hand.palmNormal();
	  kvs::Vec3 palmNormal = leap_to_World(pNormal);
	  Leap::Vector pPosition = hand.palmPosition();
	  // kvs::Vec3 palmPosition = kvs::Vec3(pPosition.x, pPosition.y, pPosition.z);
	  
	  kvs::Vec3 palmPosition_world = leap_to_World( pPosition, iBox );
	  std::cout << "p world" << std::endl;
	  palmPosition_world.print();
	  const kvs::ObjectCoordinate objectCoordinate = kvs::WorldCoordinate(palmPosition_world).toObjectCoordinate(m_volume);
	  kvs::Vec3 palmPosition = objectCoordinate.position();
	  std::cout << "p object" << std::endl;
	  palmPosition.print();
	  /* Extract planes by using SlicePlane class.                                                                                                            
	   *    c: center of gravity of the volume object.                                                                                                        
	   *    p: point located on the plane.                                                                                                                    
	   *    n: normal vector of the plane.                                                                                                                    
	   *    t: transfer function.                                                                                                                             
	   */
	  const kvs::Vector3f c( ( m_volume->maxObjectCoord() + m_volume->minObjectCoord() ) * 0.5f );
	  m_volume->minObjectCoord().print();
	  m_volume->maxObjectCoord().print();
	  const kvs::Vector3f p( c );
	  //const kvs::Vector3f p( palmPosition );
	  const kvs::Vector3f n2( palmNormal );
	  //const kvs::Vector3f n2( 1.5, 0.8, 2.0 );
	  const kvs::TransferFunction t( 256 );
	  kvs::PolygonObject* object2 = new kvs::SlicePlane( m_volume, p, n2, t );
	  
	  object2->setName("Slice");
	  m_screen.scene()->replaceObject("Slice",object2);

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
  kvs::oculus::Application app( argc, argv );
  //kvs::glut::Application app(argc, argv);
   /* Read volume data from the specified data file. If the data file is not
     * specified, scalar hydrogen volume data is created by using
     * kvs::HydrogenVolumeData class.
     */
    kvs::StructuredVolumeObject* volume = NULL;
    if ( argc > 1 ) {volume = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );
      //volume = new kvs::StructuredVectorToScalar(volume);
      Scale(volume, 10000);
      }

    }
     else            volume = new kvs::HydrogenVolumeData( kvs::Vector3ui( 64, 64, 64 ) );

    if ( !volume )
    {
        kvsMessageError( "Cannot create a structured volume object." );
        return( false );
    }

    kvs::StructuredVolumeObject* s_volume = new kvs::StructuredVectorToScalar(volume);
    /* Extract surfaces by using the Isosurface class.
     *    i: isolevel
     *    n: NormalType (PolygonNormal/VertexNormal)
     *    d: check flag whether the duplicate vertices are deleted (false) or not
     *    t: transfer function
     */
    volume->updateMinMaxValues();
    const double i = ( s_volume->maxValue() + s_volume->minValue() ) * 0.5;
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object1 = new kvs::Isosurface( s_volume, i, n, d, t );
    if ( !object1 )
    {
        kvsMessageError( "Cannot create a polygon object1." );
        delete volume;
        return( false );
    }

    object1->setName("Iso");

    /* Extract planes by using SlicePlane class.
     *    c: center of gravity of the volume object.
     *    p: point located on the plane.
     *    n: normal vector of the plane.
     *    t: transfer function.
     */
    const kvs::Vector3f c( ( volume->maxObjectCoord() + volume->minObjectCoord() ) * 0.7f );
    std::cout<<"max"<<volume->maxObjectCoord()<<std::endl;
    const kvs::Vector3f p( c );
    const kvs::Vector3f n2( 1.0, 0.8, 0.3 );
    kvs::PolygonObject* object2 = new kvs::SlicePlane( s_volume, p, n2, t );
    if ( !object2 )
      {
        kvsMessageError( "Cannot create a polygon object by Slice plane." );
        delete volume;
        return( false );
      }


    object2->setName("Slice");

    std::vector<kvs::Real32> v;

    kvs::Vector3i min_coord( 0,0,0 );
    //kvs::Vector3i min_coord( 13, 13, 13 );                                                                                                 
    //kvs::Vector3i min_coord( 7, 15,  16 );                                                                                                 
    kvs::Vector3i max_coord(63,63,63);

    for ( int k = min_coord.z(); k < max_coord.z(); k++ )

      for ( int j = min_coord.y(); j < max_coord.y(); j++ )
	{
	  for ( int i = min_coord.x(); i < max_coord.x(); i++ )
	    {
	      v.push_back( static_cast<kvs::Real32>(i) );
	      v.push_back( static_cast<kvs::Real32>(j) );
	      v.push_back( static_cast<kvs::Real32>(k) );
	    }
	}


    kvs::PointObject* point = new kvs::PointObject;
    point->setCoords( kvs::ValueArray<kvs::Real32>( v ) );
    const kvs::TransferFunction transfer_function( 256 );
    kvs::LineObject* object = new kvs::Streamline( volume, point, transfer_function );
    kvs::StylizedLineRenderer* renderer = new kvs::StylizedLineRenderer();

    renderer->enableShading();
    delete point;



    //delete volume;

    // Screen.
    kvs::oculus::Screen screen( &app );
    // kvs::glut::Screen screen( &app );
    //screen.registerObject( object);
    // screen.registerObject( object ,new kvs::Bounds() );
    screen.registerObject( object1 );
    screen.registerObject( object2 ,new kvs::Bounds() );
    screen.registerObject( object2 );
    //    screen.setGeometry( 0, 0, 512, 512 );
    screen.setTitle( "kvs::Isosurface" );
    screen.show();


    SampleListener listener( screen, volume );
    Leap::Controller controller;
    controller.addListener( listener );
    controller.setPolicy(Leap::Controller::POLICY_IMAGES);

    return( app.run() );
}
#endif