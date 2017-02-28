/******************************************************************************
*                                                                             *
*                                 deformable models                           *
*                                                                             *
******************************************************************************/
//-- fox system setup
#include "deform.h"
#include <fx3d.h>
#include <fxkeys.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include "glutils.h"

using namespace std;

#ifdef WIN32
#define SHIFT_CANVAS_ORIGIN
#endif

#define STATUSTXTLEN  2048
const FXuint TIMER_INTERVAL = 50;

// Patterns
const FXString patterns =
  "All Files (*)\n"
  "GIF Image (*.gif)\n"
  "BMP Image (*.bmp)\n"
  "XPM Image (*.xpm)\n"
  "PCX Image (*.pcx)\n"
  "RGB Image (*.rgb)\n"
//#ifdef HAVE_PNG_H
  "PNG Image (*.png)\n"
//#endif
//#ifdef HAVE_JPEG_H
  "JPEG Image (*.jpg)\n"
//#endif
  "TARGA Image (*.tga)\n"
//#ifdef HAVE_JPEG_H
  "TIFF Image (*.tif)\n"
//#endif
  ;

// Message Map
FXDEFMAP(ImageWindow) ImageWindowMap[]={

  //____Message_Type______________ID_______________Message_Handler___
  //FXMAPFUNC(SEL_PAINT, ImageWindow::ID_CANVAS, ImageWindow::onCanvasRepaint),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS, ImageWindow::ID_CANVAS, ImageWindow::onLeftMouseClick),
  FXMAPFUNC(SEL_RIGHTBUTTONPRESS, ImageWindow::ID_CANVAS, 
            ImageWindow::onRightMouseClick),
  FXMAPFUNC(SEL_MOTION, ImageWindow::ID_CANVAS, ImageWindow::onMouseMove),
  FXMAPFUNC(SEL_KEYPRESS,0,ImageWindow::onKeyPress),
  //FXMAPFUNC(SEL_KEYRELEASE,0,ImageWindow::onKeyRelease),
  FXMAPFUNC(SEL_COMMAND, ImageWindow::ID_LOAD, ImageWindow::onLoadImage),
  FXMAPFUNC(SEL_COMMAND,ImageWindow::ID_EDSENS,ImageWindow::onToggleSensorDlg),
  FXMAPFUNC(SEL_PAINT,     ImageWindow::ID_CANVAS,   ImageWindow::onExpose),
  FXMAPFUNC(SEL_CONFIGURE, ImageWindow::ID_CANVAS,   ImageWindow::onConfigure),
  FXMAPFUNC(SEL_TIMEOUT,   ImageWindow::ID_TIMEOUT,  ImageWindow::onTimeout),
  FXMAPFUNC(SEL_COMMAND,   ImageWindow::ID_OPENGL,   ImageWindow::onCmdOpenGL),
};



// Macro for the App class hierarchy implementation
FXIMPLEMENT(ImageWindow,FXMainWindow,ImageWindowMap,ARRAYNUMBER(ImageWindowMap))

bool setTitle(const std::string& title) {
    ImageWindow *mw = dynamic_cast<ImageWindow*>
        (FXApp::instance()->getRootWindow());
    if(mw) {
        mw->setTitle(title.c_str());
        return true;
    } else return false;
}

bool setStatusText(const std::string& stext) {
    ImageWindow *mw = dynamic_cast<ImageWindow*>
        (FXApp::instance()->getRootWindow());
    if(mw) {
        mw->setStatusText(stext.c_str());
        return true;
    } else return false;
}


// Construct ImageWindow
ImageWindow::ImageWindow(FXApp* a):FXMainWindow(a,"Deformable Model Segmentation",NULL,NULL,DECOR_ALL,0,0,800,600){
  FXVerticalFrame *canvasFrame;
  FXVerticalFrame *buttonFrame;
  FXHorizontalFrame *contents;

  contents=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

  canvasFrame=new FXVerticalFrame(contents,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,10,10);

  // A Visual to drag OpenGL
  glvisual=new FXGLVisual(getApp(),VISUAL_DOUBLEBUFFER);

  // Drawing glcanvas
  glcanvas=new FXGLCanvas(canvasFrame,glvisual,this,ID_CANVAS,LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);

  // RIGHT pane for the buttons
  buttonFrame=new FXVerticalFrame(contents,FRAME_SUNKEN|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,10,10);
  
  new FXButton(buttonFrame,"L&oad Image\tSelect Image to load",NULL, this, ID_LOAD,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,5,5);
//   new FXButton(buttonFrame,"&Process\tPerform some wicked image processsing",NULL,
// 				this, ID_PROCESS,
// 				FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,
// 				0,0,0,0,10,10,5,5);
  // SensorEditor button
  new FXButton(buttonFrame,"Edit &Sensors\tEdit the sensor collection",NULL,this,ID_EDSENS,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,5,5);  

  // Exit button
  new FXButton(buttonFrame,"E&xit\tQuit ImageApp",NULL,getApp(),FXApp::ID_QUIT,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,5,5);
  FXStatusBar *sbar = new FXStatusBar(canvasFrame,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  m_Statusline = sbar->getStatusLine();

  m_SensorDlg = new SensorDialog(getApp(), this, 
                                 (SensorCollection&)m_Brain.getSensors());
  // Result image*/

  filename = "untitled";
  directory = ".";

  getApp()->addTimeout(this,ID_TIMEOUT,TIMER_INTERVAL);
//   if(!m_Brain.loadModel("shape.dm"))
// 	  setStatusText("Error loading geometry.");
  //if(!m_Brain.load("default.brain"))
//	  setStatusText("Error loading brain info [default.brain].");
  m_Brain.toggleUpdateForAll();
  m_BrainStarted = false;
}

// Destroy ImageWindow
ImageWindow::~ImageWindow(){
  getApp()->removeTimeout(this,ID_TIMEOUT);
}


// Create and initialize
void ImageWindow::create(){

  // Create the windows
  FXMainWindow::create();

  if(filename!="untitled"){
	  setStatusText((FXString("trying to load: ")+filename).text());
	  if(!m_Brain.load(filename.text())) {
          setStatusText((FXString("error loading ")+filename).text());
      } else setStatusText("");
  } else if(!m_Brain.load("default.brain"))
	  setStatusText("Error loading brain info [default.brain].");
  m_SensorDlg->updateStrings();

  // Make it appear
  show(PLACEMENT_SCREEN);
}

// Handle the clear message
long ImageWindow::onCanvasRepaint(FXObject*,FXSelector,void* ptr){

// 	cerr << "onCanvasRepaint: ..."<<endl;
//     FXDCWindow	dc(glcanvas);
// //	Point pofs(glcanvas->getXPosition(), glcanvas->getYPosition());

// 	dc.setForeground(FXRGB(0,255,0));
// 	dc.drawLine(10,10,100,100);
	return 1;
}


// Widget has been resized
long ImageWindow::onConfigure(FXObject*,FXSelector,void*){
  if(glcanvas->makeCurrent()){
      glEnable(GL_BLEND);
      //glEnable(GL_SMOOTH);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glDisable(GL_DITHER);
      glViewport(0,0,glcanvas->getWidth(),glcanvas->getHeight());
      glcanvas->makeNonCurrent();
    }
  return 1;
  }



// Widget needs repainting
long ImageWindow::onExpose(FXObject*,FXSelector,void*){
  drawScene();
  return 1;
  }

// Draw the GL scene
void ImageWindow::drawScene(){
  GLdouble width = glcanvas->getWidth();
  GLdouble height = glcanvas->getHeight();
  //GLdouble aspect = height>0 ? width/height : 1.0;

  // Make context current
  glcanvas->makeCurrent();

  //glViewport(0,0,glcanvas->getWidth(),glcanvas->getHeight());

  glClearColor(0.0,0.0,0.0,1.0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if(m_Brain.getData().initialized()) {
	  GLint datwidth = m_Brain.getData().getDim1Size();
	  GLint datheight = m_Brain.getData().getDim2Size();
// 	  glPixelStorei(GL_UNPACK_ROW_LENGTH, datwidth);
// 	  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	  GLfloat xscale = ((float)width)/datwidth;
	  GLfloat yscale = ((float)height)/datheight;
	  m_XYScale = xscale<yscale?xscale:yscale;
	  GLfloat swidth = width/m_XYScale;
	  GLfloat sheight = height/m_XYScale;
	  glOrtho(0,swidth,
			  sheight,0,
			  -1000,1000);
      glEnable(GL_TEXTURE_2D);
      glColor3f(1.f, 1.f, 1.f);
	  if(m_ScreenImg.getSize()) {
          if(m_RefreshScreenImg || !m_GLScreenImg) {
              m_GLScreenImg.setImage(m_ScreenImg);
              m_RefreshScreenImg = false;
          }
          m_GLScreenImg.draw(0,0);
      //Dataset::drawImage(m_ScreenImg,0,datheight,m_XYScale,m_XYScale);
 	  } else
 		  m_Brain.getData().draw(0,0);
      glDisable(GL_TEXTURE_2D);
  }

  m_Brain.drawAllModels();

  // Swap if it is double-buffered
  if(glvisual->isDoubleBuffer()){
    glcanvas->swapBuffers();
    }

  // Make context non-current
  glcanvas->makeNonCurrent();

  if(m_BrainStarted && m_Brain.isDone()) {
	  setStatusText("Done.");
	  m_BrainStarted = false;
  }
}


// Pop a dialog showing OpenGL properties
long ImageWindow::onCmdOpenGL(FXObject*,FXSelector,void*){
  glcanvas->makeCurrent();
//  SettingsDialog sd((FXWindow*)this,glvisual);
  glcanvas->makeNonCurrent();
  //sd.execute();
  return 1;
  }

long ImageWindow::onToggleSensorDlg(FXObject*,FXSelector,void*){
    if(m_SensorDlg->shown()) m_SensorDlg->hide();
    else m_SensorDlg->show();
    return 1;
}

// Open image
long ImageWindow::onLoadImage(FXObject*,FXSelector,void*){
  FXFileDialog open(this,"Load Image (BMP,GIF,TGA)");
  
  open.setDirectory(directory);
  open.setFilename(filename);
  open.setPatternList(patterns);
  if(open.execute()){
	  filename = open.getFilename();
	  directory = open.getDirectory();
	  m_ScreenImg.setSize(0,0);
	  setStatusText((FXString("trying to load: ")+filename).text());
	  if(!m_Brain.loadData(filename.text()))
		setStatusText(("Error loading file "+filename).text());
  } 
  return 0;
}

// Key Press
long ImageWindow::onKeyPress(FXObject* obj,FXSelector sel,void* ptr){
  FXEvent* event=(FXEvent*)ptr;
  if(target && target->handle(this,MKUINT(message,SEL_KEYPRESS),ptr)) return 1;
  int _x,_y,mx,my;
  _x=m_Brain.getData().getDim1Size();
  _y=m_Brain.getData().getDim2Size();
  mx=event->win_x;
  my=event->win_y;
#ifdef SHIFT_CANVAS_ORIGIN
  mx-=glcanvas->getX();
  my-=glcanvas->getY();
#endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  if(event->code>=KEY_F5 && event->code<=KEY_F10)
  {
      static int sido=0;
	  int kid = event->code - KEY_F5;
      switch(kid) {
          case 0: sido=0; break;
          case 1: sido--; break;
          case 2: break;
          case 3: sido++; break;
      }
      sido = selectSensor(sido);
	  return 1;
  }
  if( mx < _x && my < _y ) {
      int what = event->code;
      if(what == KEY_F3) m_BrainStarted = !m_BrainStarted; // if F3
      if(m_Brain.triggerTest(mx, my, what)) return 1;
  }
  return FXMainWindow::onKeyPress(obj,sel,ptr);
}

int ImageWindow::selectSensor(int sido)
{
    vuLock glock(m_Brain.getGeomMutex());
    const SensorCollection &sensors=m_Brain.getSensors();
    const Sensor* ss=NULL;
    if((int)sensors.size()<=sido) sido=0;
    if(sido<0) sido=sensors.size()-1;
    int sid = sido;
    for(SensorCollection::const_iterator s=sensors.begin();
        s!=sensors.end();s++,sid--) {
        if(!sid) {
            ss = s->second;
            break;
        }
    }
    if(!ss || !sido) {
		setStatusText("Showing original image.");
		m_ScreenImg.setSize(0,0);
        ((SensorCollection&)sensors).selectSensor("d0");
    } else {
        if(!ss->isUpdate(Sensor::UPD_MINMAX)) {
            ((Sensor*)ss)->enableUpdate(Sensor::UPD_MINMAX);
            ((Sensor*)ss)->setModified(Sensor::UPD_MINMAX);
        }
        Image<float> simg(ss->createSensorImage());
        simg.unsetNAN(0);
        float imax=ss->getMax(); //simg.findMax();
        float imin=ss->getMin(); //simg.findMin();
        //simg+=-imin;
        //simg*=(255/(imax-imin));
        simg*=255;
        m_ScreenImg.convertFrom(simg);
        m_RefreshScreenImg = true;
        //m_ScreenImg.writePPM("sensor.ppm");
        //m_ScreenImg = m_ScreenImg.mirror(false, true);

        char msg[1024];
        sprintf(msg,"Showing sensor [%s] (min=%.4f max=%.4f).",
                ss->getID().c_str(),imin,imax);
        setStatusText(msg);
        ((SensorCollection&)sensors).selectSensor(ss->getID());
    }
    return sido;
}

// Key Release
long ImageWindow::onKeyRelease(FXObject*,FXSelector,void* ptr){
/*
  FXEvent* event=(FXEvent*)ptr;
  FXint index=current;
  flags&=~FLAG_TIP;
  if(!isEnabled()) return 0;
  if(index<0) index=0;
  switch(event->code){
    case KEY_Space:
		//blubb
  }
*/
    return 0;
}

long ImageWindow::onLeftMouseClick(FXObject*,FXSelector,void* ptr){

  FXEvent		*event=(FXEvent*)ptr;
  FXint			_x, _y, mx, my;

  Point pofs(glcanvas->getX(), glcanvas->getY());
  _x=m_Brain.getData().getDim1Size();
  _y=m_Brain.getData().getDim2Size();
  mx=event->win_x;
  my=event->win_y;
// #ifdef SHIFT_CANVAS_ORIGIN
//   mx-=glcanvas->getX();
//   my-=glcanvas->getY();
// #endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  if( mx < _x && my < _y){
	  m_Brain.triggerTest((int)(mx), 
						  (int)(my),KEY_Control_L);
  }
  return 1;
}

long ImageWindow::onRightMouseClick(FXObject*,FXSelector,void* ptr){

  FXEvent		*event=(FXEvent*)ptr;
  FXint			_x, _y, mx, my;

  Point pofs(glcanvas->getX(), glcanvas->getY());
  _x=m_Brain.getData().getDim1Size();
  _y=m_Brain.getData().getDim2Size();
  mx=event->win_x;
  my=event->win_y;
// #ifdef SHIFT_CANVAS_ORIGIN
//   mx-=glcanvas->getX();
//   my-=glcanvas->getY();
// #endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  if( mx < _x && my < _y){
	  m_Brain.triggerTest((int)(mx), 
						  (int)(my), KEY_P);
  }
  return 1;
}

long ImageWindow::onMouseMove(FXObject*,FXSelector,void* ptr){

  FXEvent		*event=(FXEvent*)ptr;
  FXint			_x, _y, mx, my;

  Point pofs(glcanvas->getX(), glcanvas->getY());
  _x=m_Brain.getData().getDim1Size();
  _y=m_Brain.getData().getDim2Size();
  mx=event->win_x;
  my=event->win_y;
// #ifdef SHIFT_CANVAS_ORIGIN
//   mx-=glcanvas->getX();
//   my-=glcanvas->getY();
// #endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  if( mx < _x && my < _y){
	  m_Brain.triggerTest((int)(mx), 
						  (int)(my),KEY_H);
  }
  return 1;
}

//  Rotate the boxes when a timer message is received
long ImageWindow::onTimeout(FXObject*,FXSelector,void*){
  drawScene();
  getApp()->addTimeout(this,ID_TIMEOUT,TIMER_INTERVAL);
  return 1;
  }

void ImageWindow::setStatusText(const char* msg, bool append)
{
  if(!append)
	m_Statusline->setNormalText(msg);
  else
	m_Statusline->setNormalText(m_Statusline->getNormalText()+FXString(msg));
}

FXbool ImageWindow::cmdline(int argc, char ** argv){

	if(argc >= 2) filename=argv[1];
	if(argc >= 3) savefilename=argv[2]; 

	return TRUE;
}

// Here we begin
int main(int argc,char *argv[]){
//#define REDIRECT_STDOUT
#ifdef REDIRECT_STDOUT
    ofstream logfile("output.log");
    cout.rdbuf(logfile.rdbuf());
    cerr.rdbuf(logfile.rdbuf());
#endif
  // Make application
  FXApp application("Deformable Model Segmentation","FoxText");

  // Start app
  application.init(argc,argv);

  // Make window
  ImageWindow *iw = new ImageWindow(&application);
  iw->cmdline(argc,argv);

  glutInit(&argc,argv);

  // Create the application's windows
  application.create();

  // Run the application
  return application.run();
}

int gethostname(char*,int)
{
    return 0;
}

/* emacs layout configuration
;;; Local Variables: ***
;;; tab-width: 4 ***
;;; End: ***
*/
