#include <kvs/OpenGL>
#include <Lib/Oculus.h>
#include <Lib/HeadMountedDisplay.h>
#include <Lib/Application.h>
#include <Lib/Screen.h>
#include <iostream>

#include <kvs/HydrogenVolumeData>

#include <kvs/StructuredVolumeObject>
#include <kvs/StructuredVolumeImporter>
#include <kvs/StructuredVectorToScalar>
#include <kvs/PolygonObject>
#include <kvs/Isosurface>
#include <kvs/LineObject>
#include <kvs/LineRenderer>
#include <kvs/StylizedLineRenderer>
#include <kvs/Streamline>
#include <kvs/SlicePlane>
#include <kvs/Bounds>
#include <Leap.h>

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

kvs::Vec3 toVec3(Leap::Vector& v)
{
  return kvs::Vec3(v.x, v.y, v.z);
}

struct CaptureData{
  
  kvs::Mat4 leapPose;
  Leap::Frame frame;
};
class LeapHandler : public Leap::Listener
{
private:

  bool hasFrame;
  //std::thread captureThread;
  std::mutex mutex;
  kvs::glut::Screen& m_screen;
  kvs::StructuredVolumeObject* m_volume;
  Leap::Controller controller;
  //ovrHmd hmd;
  CaptureData frame;
public:

  LeapHandler( kvs::glut::Screen& screen, kvs::StructuredVolumeObject* volume ) : m_screen( screen ), m_volume( volume ) {}
  
  void startCapture()
  {
    controller.addListener(*this);
  }
  void stopCapture()
  {
    controller.removeListener(*this);
  }
  void set(const CaptureData & newFrame) {
    std::lock_guard<std::mutex> guard(mutex);
    frame = newFrame;
    hasFrame = true;
  }
  bool get(CaptureData & out) {
    std::lock_guard<std::mutex> guard(mutex);
    if (!hasFrame) {
      return false;
    }
    out = frame;
    hasFrame = false;
    return true;
  }
  void onInit( const Leap::Controller& controller )
  {
    std::cout << "Init" << std::endl;
  }

  void  onConnect(const Leap::Controller& controller)
  {
    controller.setPolicy
      (Leap::Controller::PolicyFlag::POLICY_OPTIMIZE_HMD);
      
    std::cout << "Connected" << std::endl;

  }


  
  void onFrame(const Leap::Controller& controller)
  {
    CaptureData frame;
    frame.frame = controller.frame();
    double time = kvs::oculus::TimeInSecond();
    ovrTrackingState state = (new kvs::oculus::HeadMountedDisplay)->trackingState(time);
    ovrPosef headPose = state.HeadPose.ThePose;
    frame.leapPose = kvs::oculus::ToMat4(OVR::Matrix4f::Translation(headPose.Position)*OVR::Matrix4f(headPose.Orientation));
    set(frame);
    Leap::HandList hands = frame.frame.hands();
    for(int iHand = 0; iHand < hands.count();iHand++)
      {
	Leap::Hand hand = hands[iHand];
        //Leap::Finger finger = hand.fingers()[1];                                                                                                              
        //Leap::FingerList indexFingerList = hand.fingers().fingerType(Leap::Finger::TYPE_INDEX);                                                               
	Leap::Finger thumb = hand.fingers()[0];
	Leap::Finger index = hand.fingers()[1];
	Leap::Finger middle = hand.fingers()[2];
        if(hand.isValid() && index.isExtended())
          {
            if(middle.isExtended())
              {
                Sliceplane(m_screen, m_volume, hand);
		if(middle.isExtended()==false)
		  {
		    //controller.removeListener( listener );
		  }
		
              }
            else if(thumb.isExtended())
              {
                Isosurface(m_screen, m_volume, thumb, index);
		
              }
	    Streamline(m_screen, index);
	    
          }
      }
    m_screen.redraw();
  }
};

/*
class LeapApp 
{
protected:
  LeapHandler captureHandler;
  CaptureData latestFrame;
public:
  
  LeapApp() : captureHandler(screen, volume);
  {
    captureHandler.startCapture();
  }
  virtual ~LeapApp()
  {
    captureHandler.stopCapture();
  }
  virtual void update()
  {
    Leap::HandList hands = latestFrame.frame.hands();
    for(int iHand = 0; iHand < hands.count();iHand++)
      {
	Leap::Hand hand = hands[iHand];
	//Leap::Finger finger = hand.fingers()[1];
	//Leap::FingerList indexFingerList = hand.fingers().fingerType(Leap::Finger::TYPE_INDEX);
	Leap::Finger thumb = hand.fingers()[0];
	Leap::Finger index = hand.fingers()[1];
	Leap::Finger middle = hand.fingers()[2];
	if(hand.isValid() && index.isExtended())
	  {
	    if(middle.isExtended())
	      {
		void slicePlane(hand);
	      }
	    else if(thumb.isExtended()) 
	      {
		void isoSurface(thumb, index);
	      }
	    void StreamLine(index);
	  }
      }
  }
  
}；
*/
kvs::Vec3 leapToRift(Leap::Vector& vec)
{
  return kvs::Vec3(-vec.x, -vec.z, -vec.y);
}

kvs::Vec3 leapToRiftPosition(Leap::Vector& vec)
{
  return leapToRift(vec)/1000.0f + kvs::Vec3(0, 0, -0.080);
}


void Streamline(kvs::glut::Screen& screen , kvs::StructuredVolumeObject* volume)
{
  Scale(volume, 10000);
  std::cout<<"min= "<<volume->minValue()<<std::endl;
  std::cout<<"max= "<<volume->maxValue()<<std::endl;

  std::vector<kvs::Real32> v;
  kvs::Vector3i min_coord(0,0,0 );
  kvs::Vector3i max_coord(17,17,17);

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
  screen.registerObject( object, new kvs::Bounds() );
  screen.registerObject( object, renderer );
  screen.registerObject( object );

}

//interactive update
void StreamLine(kvs::glut::Screen& screen , kvs::StructuredVolumeObject* volume, Leap::Finger index) 
{
  Scale(volume, 10000);
  std::cout<<"min= "<<volume->minValue()<<std::endl;
  std::cout<<"max= "<<volume->maxValue()<<std::endl;

  std::vector<kvs::Real32> v;
  kvs::Vector3i min_coord(0,0,0 );
  kvs::Vector3i max_coord(17,17,17);

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
  screen.registerObject( object, new kvs::Bounds() );
  screen.registerObject( object, renderer );
  screen.registerObject( object );

}

void Sliceplane(kvs::glut::Screen& screen , kvs::StructuredVolumeObject* volume)
{
  kvs::StructuredVolumeObject* s_volume = new kvs::HydrogenVolumeData( kvs::Vec3ui( 64, 64, 64 ) );
  const kvs::Vector3f c( ( volume->maxObjectCoord() + volume->minObjectCoord() ) * 0.5f );
  const kvs::Vector3f p( c );
  const kvs::Vector3f n( 1.5, 0.8, 2.0 );
  const kvs::TransferFunction t( 256 );
  kvs::PolygonObject* object = new kvs::SlicePlane( s_volume, p, n, t );
  screen.registerObject( object, new kvs::Bounds() );
  screen.registerObject( object );
  object->setName("slice");
}
void SlicePlane(kvs::glut::Screen& screen , kvs::StructuredVolumeObject* volume, Leap::Hand hand)
{
  kvs::StructuredVolumeObject* s_volume = new kvs::HydrogenVolumeData( kvs::Vec3ui( 64, 64, 64 ) );
  const kvs::Vector3f c( ( volume->maxObjectCoord() + volume->minObjectCoord() ) * 0.5f );
  const kvs::Vector3f p( c );
  const kvs::Vector3f n( 1.5, 0.8, 2.0 );
  const kvs::TransferFunction t( 256 );
  kvs::PolygonObject* object = new kvs::SlicePlane( s_volume, p, n, t );
  screen.registerObject( object, new kvs::Bounds() );
  screen.registerObject( object );
}

void IsoSurface( kvs::glut::Screen& screen , kvs::StructuredVolumeObject* volume , Leap::Finger thumb, Leap::Finger indexFinger)
{

  
  kvs::StructuredVolumeObject* s_volume = new kvs::StructuredVectorToScalar(volume);
  
  float distance = (thumb.tipPosition()).distanceTo(indexFinger.tipPosition()); 
  const double i = volume->maxValue()*distance * 0.0125;
  //const double i = kvs::Math::Mix( volume->minValue(), volume->maxValue(), 0.5 );
  const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
  const bool d = false;
  const kvs::TransferFunction t( 256 );
  kvs::PolygonObject* object = new kvs::Isosurface( s_volume, i, n, d, t );
  //delete volume;                                                                                                                                            

  screen.registerObject( object, new kvs::Bounds() );
  screen.registerObject( object );
  object->setName("Iso");
  screen.scene()->replaceObject("Iso",object);
}
void Isosurface( kvs::glut::Screen& screen , kvs::StructuredVolumeObject* volume)
{


  kvs::StructuredVolumeObject* s_volume = new kvs::StructuredVectorToScalar(volume);

  const double i = kvs::Math::Mix( volume->minValue(), volume->maxValue(), 0.2 );
  const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
  const bool d = false;
  const kvs::TransferFunction t( 256 );
  kvs::PolygonObject* object = new kvs::Isosurface( s_volume, i, n, d, t );
  //delete volume;                                                                                                                                            

  screen.registerObject( object, new kvs::Bounds() );
  screen.registerObject( object );
  object->setName("Iso");
  LeapHandler handler(screen, volume);
}
/*
void Isosurface( kvs::oculus::Screen& screen )
{
    kvs::StructuredVolumeObject* volume = new kvs::HydrogenVolumeData( kvs::Vec3ui( 64, 64, 64 ) );

    const double i = kvs::Math::Mix( volume->minValue(), volume->maxValue(), 0.2 );
    const kvs::PolygonObject::NormalType n = kvs::PolygonObject::VertexNormal;
    const bool d = false;
    const kvs::TransferFunction t( 256 );
    kvs::PolygonObject* object = new kvs::Isosurface( volume, i, n, d, t );
    

    screen.registerObject( object, new kvs::Bounds() );
    screen.registerObject( object );
    LeapHandler handler(screen, volume);
}

#include <kvs/RayCastingRenderer>

void VolumeRendering( kvs::glut::Screen& screen )
{
    kvs::StructuredVolumeObject* object = new kvs::HydrogenVolumeData( kvs::Vec3ui( 64, 64, 64 ) );

//    kvs::glsl::RayCastingRenderer* renderer = new kvs::glsl::RayCastingRenderer();
    kvs::RayCastingRenderer* renderer = new kvs::RayCastingRenderer();

    screen.registerObject( object, new kvs::Bounds() );
    screen.registerObject( object, renderer );
    
    
}
*/

int main( int argc, char** argv )
{
    kvs::oculus::Application app( argc, argv );
    kvs::oculus::Screen screen( &app );
    screen.show();
    kvs::StructuredVolumeObject* volume = NULL;
    if ( argc > 1 ) volume = new kvs::StructuredVolumeImporter( std::string( argv[1] ) );

    else volume = new kvs::HydrogenVolumeData( kvs::Vec3ui( 64, 64, 64 ) );


    Isosurface( screen, volume );
  
    LeapHandler handler( screen, volume );
    Leap::Controller controller;
    controller.addListener( handler );
  
//    VolumeRendering( screen );

//    screen.show();

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
