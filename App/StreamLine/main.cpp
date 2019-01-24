#include <kvs/OpenGL>
#include <Lib/Oculus.h>
#include <Lib/HeadMountedDisplay.h>
#include <Lib/Application.h>
#include <Lib/Screen.h>
#include <iostream>

#include <kvs/Message>
#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/Streamline>
#include <kvs/TornadoVolumeData>
//#include <kvs/glut/Application>
//#include <kvs/glut/Screen> 
#include <Leap.h>
#include <unistd.h>
#include <kvs/Bounds>
#include <kvs/StylizedLineRenderer>


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
    controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
  }
  kvs::Vec3 leap_to_World(Leap::Vector leapPoint, Leap::InteractionBox iBox )
  {
    std::cout<<"--------------------------------------------------------------"<<std::endl;
    std::cout<<"LeapPoint position"<<leapPoint.x<<" "<<leapPoint.y<<" "<<leapPoint.z<<std::endl;
    kvs::Vec3 v = toVec3(leapPoint);
    std::cout<<"before"<<std::endl;
    v.print();
    if(v.x()==0 && v.y()==0 && v.z()==0)
      {
	return v;
      }
    
      Leap::Vector normalized  = iBox.normalizePoint(leapPoint);
      std::cout<<"noramalized"<<std::endl;
      std::cout<<normalized.x<<" "<<normalized.y<<" "<<normalized.z<<std::endl;
      v = toVec3(normalized);
      //v += ( m_volume->maxObjectCoord() + m_volume->minObjectCoord() ) * 0.5f ;
      v = v * 31;
      std::cout<<"after"<<std::endl;
      v.print();
      //kvs::Vec4 v_t1 = kvs::Vec4(v,1);
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
                   0, -1, 0, 0,//-0.08,
                   0, 0, 0, 1
                   );
    kvs::Mat4 M_lw = M_ow* M_lo;
    kvs::Vec4 v_t = kvs::Vec4(v,1);
    v_t = M_lw* v_t;
    v = v_t.xyz();
    std::cout<<"leap_oculusworld"<<std::endl;
    v.print();
   
    return v;
    
  }
    kvs::Vec3 toVec3(Leap::Vector& v)
  {
    return kvs::Vec3(v.x, v.y, v.z);
  }

  void onFrame( const Leap::Controller& controller )
  {
    const Leap::Frame frame = controller.frame();
    //std::cout << "Frame id:" << frame.id() << std::endl;
    Leap::InteractionBox interactionBox = frame.interactionBox();
   
    //const Leap::GestureList gestures = frame.gestures();

    //std::cout << "gestures.count() = " << gestures.count () << std::endl;
    
    //InteractionBox
    Leap::InteractionBox iBox = frame.interactionBox(); 
    //Leap::Gesture gesture = gestures[g];
    //Leap::ScreenTapGesture screentap = gesture;
    usleep(1000);
    Leap::Pointable pointable = frame.pointables().frontmost();
    Leap::Vector stabilizedPosition = pointable.stabilizedTipPosition();
    kvs::Vec3 position = leap_to_World(stabilizedPosition, iBox);
    //Leap::Vector position = screentap.position();
	
    std::vector<kvs::Real32> v;
    v.push_back( position.x() );
    v.push_back( position.y() );
    v.push_back( position.z() );
    
      
    /*
      kvs::Vector3i min_coord( stabilizedPosition.x*0.01, stabilizedPosition.y*0.01, stabilizedPosition.z*0.01);
	std::cout << "test" <<std::endl;
	//std::cout << position.x<< " "<< position.y << " "<< position.z <<std::endl;
	
	kvs::Vector3i max_coord( 20, 20, 30);
	for ( int k = min_coord.z(); k < max_coord.z(); k++ )
	{
	for ( int j = min_coord.y(); j < max_coord.y(); j++ )
	{
	for ( int i = min_coord.x(); i < max_coord.x(); i++ )
	{
	v.push_back( static_cast<kvs::Real32>(i) );
	v.push_back( static_cast<kvs::Real32>(j) );
		    v.push_back( static_cast<kvs::Real32>(k) );
		    }
	      }
	      }
    */
    kvs::PointObject* point = new kvs::PointObject;
    point->setCoords( kvs::ValueArray<kvs::Real32>( v ) );
    const kvs::TransferFunction transfer_function( 256 );
    kvs::LineObject* object = new kvs::Streamline( m_volume, point, transfer_function );
    kvs::StylizedLineRenderer* renderer = new kvs::StylizedLineRenderer();
    renderer->enableShading();
    object->setName("Stream");
    
    //object->print( std::cout );
    
    delete point;
    
    if ( m_screen.scene()->hasObject( "Stream") )
	  {
	    m_screen.scene()->replaceObject("Stream",object);
	  }
    else
      {
	m_screen.registerObject( object );
      }
  
  m_screen.redraw();
  }
};

int main( int argc, char** argv )
{
    kvs::oculus::Application app( argc, argv );
    /* Read volume data from the specified data file. If the data file is not
     * specified, scalar hydrogen volume data is created by using
     * kvs::HydrogenVolumeData class.
     */
    kvs::StructuredVolumeObject* volume = NULL;
    if ( argc > 1 ) volume = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );
    else            volume = new kvs::TornadoVolumeData( kvs::Vector3ui( 32, 32, 32 ) );
    /*
    std::vector<kvs::Real32> v;
    kvs::Vector3i min_coord( 15, 15,  0 );
    kvs::Vector3i max_coord( 20, 20, 30 );
    for ( int k = min_coord.z(); k < max_coord.z(); k++ )
    {
        for ( int j = min_coord.y(); j < max_coord.y(); j++ )
        {
            for ( int i = min_coord.x(); i < max_coord.x(); i++ )
            {
                v.push_back( static_cast<kvs::Real32>(i) );
                v.push_back( static_cast<kvs::Real32>(j) );
                v.push_back( static_cast<kvs::Real32>(k) );
            }
        }
    }
    kvs::PointObject* point = new kvs::PointObject;
    point->setCoords( kvs::ValueArray<kvs::Real32>( v ) );
    
    const kvs::TransferFunction transfer_function( 256 );
    kvs::LineObject* object = new kvs::Streamline( volume, point, transfer_function );
    object->setName("Stream");
    
    // delete volume;
    // delete point;
    */
        
    kvs::oculus::Screen screen( &app );
    
    screen.registerObject( volume, new kvs::Bounds());
    //    screen.registerObject( object);
    
    screen.show();
    
    SampleListener listener( screen, volume );
    Leap::Controller controller;
    controller.addListener( listener );
   
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
