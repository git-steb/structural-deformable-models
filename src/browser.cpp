/******************************************************************************
*                                                                             *
*                                 database browser                            *
*                                                                             *
******************************************************************************/
//-- fox system setup
#include <fx.h>
#include <fx3d.h>
#include <fxkeys.h>
#include <string>
#include <iostream>
#include <sstream>
#include <time.h>
#include "fxconfig.h"

#include <list>
#include "Image.h"
#include "Data.h"
#include "SpeciesDB.h"
#include "Clusterer.h"
#include "SensorSet.h"

using namespace std;

// Patterns
const FXchar *patterns[]={
  "Databases",   "*.dat",
  "All Files",   "*",
#ifdef LOADIMAGES
  "GIF Image",   "*.gif",
  "BMP Image",   "*.bmp",
  "XPM Image",   "*.xpm",
  "PCX Image",   "*.pcx",
  "RGB Image",   "*.rgb",
//#ifdef HAVE_PNG_H
  "PNG Image",   "*.png",
//#endif
//#ifdef HAVE_JPEG_H
  "JPEG Image",  "*.jpg",
//#endif
  "TARGA Image", "*.tga",
//#ifdef HAVE_JPEG_H
  "TIFF Image",  "*.tif",
//#endif
#endif
  NULL
  };

/*******************************************************************************/
// Event Handler Object
class ImageWindow : public FXMainWindow {

  // Macro for class hierarchy declarations
  FXDECLARE(ImageWindow)

private:

    FXGLCanvas	  *glcanvas;				// glcanvas
	FXBMPImage      *picture;                   // Complete picture
	FXFont          *font;                      // Font for text

	FXString        filename;                   // File being viewed
	FXString        savefilename;               // File for results
    FXString	  directory;				  // Last visited directory

	Dataset				m_Data;
	Image<byte>			m_ScreenImg;
    SpeciesDB           m_DB;                   // Database of species
    Species             m_CSpecies;
    float               m_Unit;
    //list<string>      m_Records;
    //list<string>::iterator      m_CRec;
    DBSelector          m_Selector;
	
	FXGLVisual        *glvisual;                // Visual for OpenGL

	float			m_XYScale;
    FXStatusline    *m_Statusline;
protected:
	ImageWindow() : m_Selector(m_DB) {}

public:

  // Message handler
  long onCanvasRepaint(FXObject*,FXSelector,void*);
  long onLeftMouseClick(FXObject*,FXSelector,void*);
  long onRightMouseClick(FXObject*,FXSelector,void*);
  long onKeyPress(FXObject*,FXSelector,void* ptr);
  long onKeyRelease(FXObject*,FXSelector,void* ptr);
  long onCmdRestore(FXObject*,FXSelector,void*);
  long onLoadImage(FXObject*,FXSelector,void*);
  long onMouseDown(FXObject*,FXSelector,void*);
  long onMouseUp(FXObject*,FXSelector,void*);
  long onMouseMove(FXObject*,FXSelector,void*);
  long onExpose(FXObject*,FXSelector,void*);
  long onConfigure(FXObject*,FXSelector,void*);
  long onCmdOpenGL(FXObject*,FXSelector,void*);

  // Draw scene
  void drawScene();
  void changeSpecies(const Species &s);
  void setStatusText(const char* msg, bool append=false);

public:

  // Messages for our class
  enum{
      ID_CANVAS=FXMainWindow::ID_LAST,
		  ID_TEST,
		  ID_LOAD,
		  ID_OPENGL,
		  ID_LAST
		  };
	
public:

  // ImageWindow constructor
  ImageWindow(FXApp* a);

  // Initialize
  virtual void create();

  // Read command line
  FXbool cmdline(int argc, char ** argv);

  // ImageWindow destructor
  virtual ~ImageWindow();

};



// Message Map
FXDEFMAP(ImageWindow) ImageWindowMap[]={

  //____Message_Type______________ID_______________Message_Handler___
  //FXMAPFUNC(SEL_PAINT, ImageWindow::ID_CANVAS, ImageWindow::onCanvasRepaint),
  FXMAPFUNC(SEL_LEFTBUTTONPRESS, ImageWindow::ID_CANVAS, ImageWindow::onLeftMouseClick),
  FXMAPFUNC(SEL_RIGHTBUTTONPRESS, ImageWindow::ID_CANVAS, ImageWindow::onRightMouseClick),
  FXMAPFUNC(SEL_KEYPRESS,0,ImageWindow::onKeyPress),
  //FXMAPFUNC(SEL_KEYRELEASE,0,ImageWindow::onKeyRelease),
  FXMAPFUNC(SEL_COMMAND, ImageWindow::ID_LOAD, ImageWindow::onLoadImage),
  FXMAPFUNC(SEL_PAINT,     ImageWindow::ID_CANVAS,   ImageWindow::onExpose),
  FXMAPFUNC(SEL_CONFIGURE, ImageWindow::ID_CANVAS,   ImageWindow::onConfigure),
  FXMAPFUNC(SEL_COMMAND,   ImageWindow::ID_OPENGL,   ImageWindow::onCmdOpenGL),
};



// Macro for the App class hierarchy implementation
FXIMPLEMENT(ImageWindow,FXMainWindow,ImageWindowMap,ARRAYNUMBER(ImageWindowMap))


bool setTitle(const char* title) {
    return false;
    ImageWindow *mw = dynamic_cast<ImageWindow*>
        (FXApp::instance()->getMainWindow());
    if(mw) {
        mw->setTitle(title);
        return true;
    } else return false;
}

bool setStatusText(const char* stext) {
    return false;
    ImageWindow *mw = dynamic_cast<ImageWindow*>
        (FXApp::instance()->getMainWindow());
    if(mw) {
        mw->setStatusText(stext);
        return true;
    } else return false;
}

// Construct ImageWindow
ImageWindow::ImageWindow(FXApp* a):FXMainWindow(a,"Deformable Model Segmentation",NULL,NULL,DECOR_ALL,0,0,800,600), m_Selector(m_DB) {
  FXVerticalFrame *canvasFrame;
  FXVerticalFrame *buttonFrame;
  FXHorizontalFrame *contents;

  contents=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);

  canvasFrame=new FXVerticalFrame(contents,FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,10,10);

  // A Visual to drag OpenGL
  glvisual=new FXGLVisual(getApp(),VISUAL_DOUBLEBUFFER|VISUAL_STEREO);

  // Drawing glcanvas
  glcanvas=new FXGLCanvas(canvasFrame,glvisual,this,ID_CANVAS,LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT);

  // RIGHT pane for the buttons
  buttonFrame=new FXVerticalFrame(contents,FRAME_SUNKEN|LAYOUT_FILL_Y|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,10,10);
  
  new FXButton(buttonFrame,"L&oad Image\tSelect Image to load",NULL, this, ID_LOAD,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,5,5);
//   new FXButton(buttonFrame,"&Process\tPerform some wicked image processsing",NULL,
// 				this, ID_PROCESS,
// 				FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,
// 				0,0,0,0,10,10,5,5);

  // Exit button
  new FXButton(buttonFrame,"E&xit\tQuit ImageApp",NULL,getApp(),FXApp::ID_QUIT,FRAME_THICK|FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_TOP|LAYOUT_LEFT,0,0,0,0,10,10,5,5);
  FXStatusbar *sbar = new FXStatusbar(canvasFrame,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X);
  m_Statusline = sbar->getStatusline();
  
  // Result image*/
  picture=new FXBMPImage(getApp(),"test.rgb",IMAGE_SHMI|IMAGE_SHMP,850,600);
  
  // Make font
  font=new FXFont(getApp(),"times",36,FONTWEIGHT_BOLD);

  // Make a tip
  /*new FXTooltip(getApp());*/

  filename = "untitled";
  directory = ".";
  m_Unit = 1;
}

// Destroy ImageWindow
ImageWindow::~ImageWindow(){

  if(picture) {
	  delete picture;
	  picture=NULL;
  }
  if(font) {
	  delete font;
	  font=NULL;
  }
}


// Create and initialize
void ImageWindow::create(){

  // Create the windows
  FXMainWindow::create();

  picture->create();

  // Font too
  font->create();

//   ifstream rf("records.list");
//   if(rf) {
//       string rec;
//       while(getline(rf, rec)) 
//           m_Records.push_back(rec);
//       cout << m_Records.size() << " records read from records.list" << endl;
//       m_DB.load(m_Records.begin()->c_str());
//       changeSpecies(m_DB.begin()->second);
//   } else cout << "no records.list loaded" << endl;
//   m_CRec = m_Records.begin();

  if(!m_Selector.load("kinds.list"))
//   m_DB.load(m_Records.begin()->c_str());
//       changeSpecies(m_DB.begin()->second);
//   } else cout << "no records.list loaded" << endl;
//   m_CRec = m_Records.begin();

  if(filename!="untitled"){
	  setStatusText((FXString("trying to load: ")+filename).text());
	  if(!m_DB.load(filename.text())) {
          setStatusText((FXString("error loading ")+filename).text());
      } else {
          setStatusText("");
          //m_Records.push_back(string(filename.text()));
          cout << "dir: " << directory.text() << endl;
          changeSpecies(m_DB.begin()->second);
      }
  } 
  // Make it appear
  show(PLACEMENT_SCREEN);
}

// Handle the clear message
long ImageWindow::onCanvasRepaint(FXObject*,FXSelector,void* ptr){

	//todo: take scroll bar offsets into account to position circle centers

	cerr << "onCanvasRepaint: ..."<<endl;

	FXDCWindow	dc(glcanvas);
//	Point pofs(glcanvas->getXPosition(), glcanvas->getYPosition());

	dc.setForeground(FXRGB(0,255,0));
	dc.drawLine(10,10,100,100);
	return 1;
}


// Widget has been resized
long ImageWindow::onConfigure(FXObject*,FXSelector,void*){
  if(glcanvas->makeCurrent()){
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

  glViewport(0,0,glcanvas->getWidth(),glcanvas->getHeight());

  glClearColor(0.0,0.0,0.0,1.0);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_DITHER);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if(m_Data.initialized()) {
	  GLint datwidth = m_Data.getDim1Size();
	  GLint datheight = m_Data.getDim2Size();
	  glPixelStorei(GL_UNPACK_ROW_LENGTH, datwidth);
	  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	  GLfloat xscale = ((float)width)/datwidth;
	  GLfloat yscale = ((float)height)/datheight;
	  m_XYScale = xscale<yscale?xscale:yscale;
	  GLfloat swidth = width/m_XYScale;
	  GLfloat sheight = height/m_XYScale;
	  glOrtho(0,swidth,
			  sheight,0,
			  -1000,1000);
// 	  if(m_ScreenImg.getSize())
// 		  Dataset::drawImage(m_ScreenImg,0,datheight,m_XYScale,m_XYScale);
// 	  else
		  m_Data.draw(0,datheight,m_XYScale,m_XYScale);
  }

  // Swap if it is double-buffered
  if(glvisual->isDoubleBuffer()){
    glcanvas->swapBuffers();
    }

  // Make context non-current
  glcanvas->makeNonCurrent();

}


// Pop a dialog showing OpenGL properties
long ImageWindow::onCmdOpenGL(FXObject*,FXSelector,void*){
  glcanvas->makeCurrent();
//  SettingsDialog sd((FXWindow*)this,glvisual);
  glcanvas->makeNonCurrent();
  //sd.execute();
  return 1;
  }

// Open image
long ImageWindow::onLoadImage(FXObject*,FXSelector,void*){
  FXFileDialog open(this,"Load Database (DAT)");
  
  open.setDirectory(directory);
  open.setFilename(filename);
  open.setPatternList(patterns);
  if(open.execute()){
	  filename = open.getFilename();
	  directory = open.getDirectory();
	  m_ScreenImg.setSize(0,0);
	  setStatusText((FXString("trying to load: ")+filename).text());
	  if(!m_DB.load(filename.text())) {
          setStatusText("Error loading file.");
      } else {
          setStatusText("");
          changeSpecies(m_DB.begin()->second);
      }
  } 
  return 0;
}

void ImageWindow::changeSpecies(const Species &s) 
{
    m_CSpecies = s;
    directory = m_DB.getDirectory().c_str();
    if(!m_Data.load((directory+"\\"+m_CSpecies["image"].c_str()).text()))
    {
        cout << "Error loading image "
             << (directory+"\\"+m_CSpecies["image"].c_str()).text() << endl;
    }
    string msg = "species_id: "; 
    stringstream sstrm;
    sstrm << m_CSpecies.getScale();
    string number;
    sstrm >> number;
    msg += m_CSpecies["species_id"] + "  image: " + m_CSpecies["image"]
        + " flags: " + m_CSpecies.getFlagString() 
        + " scale (pixels per mm): "+number;
    setStatusText(msg.c_str());
    setTitle(m_CSpecies["name"].c_str());
    drawScene();
    //cout << m_CSpecies;
}

// Key Press
long ImageWindow::onKeyPress(FXObject* obj,FXSelector sel,void* ptr){
  FXEvent* event=(FXEvent*)ptr;
  if(target && target->handle(this,MKUINT(message,SEL_KEYPRESS),ptr)) return 1;
  int _x,_y,mx,my;
  _x=m_Data.getDim1Size();
  _y=m_Data.getDim2Size();
  mx=event->win_x;
  my=event->win_y;
#ifdef WIN32
  mx-=glcanvas->getX();
  my-=glcanvas->getY();
#endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  int walkdir=0;
  switch(event->code) {
      case KEY_space:
          changeSpecies(m_CSpecies);
          break;
      case KEY_Right:
      {
          if(m_Selector.nextSelection(1))
              changeSpecies(m_Selector.getCurSpecies());
          break;
      }
      case KEY_Left:
      {
          if(m_Selector.nextSelection(-1))
              changeSpecies(m_Selector.getCurSpecies());
          break;
      }
      case KEY_Up:
          walkdir=-1;
      case KEY_Down:
      {
          if(!walkdir) walkdir=1;
          if(m_Selector.nextSelector(walkdir))
              changeSpecies(m_Selector.getCurSpecies());
//           if(m_DB.load(m_CRec->c_str())) {
//               cout << *m_CRec << " loaded." << endl;
//               FXString filename(m_CRec->c_str());
//               int ps = filename.findb('\\');
//               if(ps == -1) ps = filename.findb('/');
//               if(ps == -1) directory = ".";
//               else directory = filename.left(ps);
//               changeSpecies(m_DB.begin()->second);
//           } else cout << "error loading " << *m_CRec << endl;
          break;
      }
      case KEY_F1: case KEY_F2: case KEY_F3: case KEY_F4: case KEY_F5: 
      case KEY_F6: case KEY_F7: case KEY_F8: case KEY_F9: 
      {
          static const int nct[9] = {1,2,3,4,5,10,30,50,100};
          int nc = nct[event->code-KEY_F1];
          cout << "computing " << nc << " nearest colours." << endl;
          Clusterer<3> clusterer(nc,0,255);
          clusterer.m_Elements.reserve(m_Data.getDim2Size()
                                       *m_Data.getDim1Size());
          //add elements
          int i,j;
          int height = m_Data.getDim2Size();
          for(j=0; j<m_Data.getDim2Size(); j++)
              for(i=0; i<m_Data.getDim1Size(); i++)
              {
                  CElement<3> col(m_Data.getMValue(i,height-j-1));
                  clusterer.m_Elements.push_back(col);
              }
          clusterer.iterateClassification(30);
          //generate colour clustered Image
          int size = int(m_Data.getDim1Size()*m_Data.getDim2Size());
          for(i=0; i<size; i++)
          {
              const CElement<3> &pc=clusterer.m_Classes[
                  clusterer.m_Elements[i].getClassID()];
              m_Data.getImage(0)[i] = (byte)pc.val[0];
              m_Data.getImage(1)[i] = (byte)pc.val[1];
              m_Data.getImage(2)[i] = (byte)pc.val[2];
          }
          drawScene();
          break;
      }
      case KEY_c:
      {
          cout << "computing chrominance." << endl;
          int i,j,ind=0;
          int height = m_Data.getDim2Size();
          for(j=0; j<m_Data.getDim2Size(); j++)
              for(i=0; i<m_Data.getDim1Size(); i++,ind++)
              {
                  vector<float> c(m_Data.getMValue(i,height-j-1));
#define SATURATION
//#define AGD
#ifdef AGD
                  float g = (c[0]+c[1]+c[2])/3.0f;
                  float R = fabs(g-c[1])+abs(g-c[2]);
                  float G = fabs(g-c[2])+abs(g-c[0]);
                  float B = fabs(g-c[0])+abs(g-c[1]);
#elif defined(SATURATION) 
                  float v,m;
                  v = max(c[0],max(c[1],c[2]));
                  m = min(c[0],min(c[1],c[2]));
                  float G; // = v;
                  float R,B;
                  G = R = B = (v>0)?255*(v-m)/v : 0;
                  //G = R = B = v-m;
#endif
                  m_Data.getImage(0)[ind] = (byte)R;
                  m_Data.getImage(1)[ind] = (byte)G;
                  m_Data.getImage(2)[ind] = (byte)B;
              }
          drawScene();
          break;
      }
      case KEY_W:
      {
          ofstream os((m_DB.getDirectory()+"//species-db.dat").c_str());
          os << m_DB;
          break;
      }
      case KEY_f:
          m_DB[m_CSpecies.id].setFlag(Species::FLIPPED);
          break;
      case KEY_F:
          m_DB[m_CSpecies.id].unsetFlag(Species::FLIPPED);
          break;
      case KEY_s:
          m_DB[m_CSpecies.id].setFlag(Species::SCALE);
          break;
      case KEY_S:
          m_DB[m_CSpecies.id].unsetFlag(Species::SCALE);
          break;
      case KEY_d:
          m_DB[m_CSpecies.id].setFlag(Species::DEL);
          break;
      case KEY_D:
          m_DB[m_CSpecies.id].unsetFlag(Species::DEL);
          break;
      case KEY_1:
          m_DB[m_CSpecies.id].setFlag(Species::SEL1);
          break;
      case KEY_exclam:
          m_DB[m_CSpecies.id].unsetFlag(Species::SEL1);
          break;
      case KEY_2:
          m_DB[m_CSpecies.id].setFlag(Species::SEL2);
          break;
      case KEY_at:
          m_DB[m_CSpecies.id].unsetFlag(Species::SEL2);
          break;
      case KEY_3:
          m_DB[m_CSpecies.id].setFlag(Species::SEL3);
          break;
      case KEY_numbersign:
          m_DB[m_CSpecies.id].unsetFlag(Species::SEL3);
          break;
      case KEY_4:
          m_DB[m_CSpecies.id].setFlag(Species::SEL4);
          break;
      case KEY_dollar:
          m_DB[m_CSpecies.id].unsetFlag(Species::SEL4);
          break;
      case KEY_5:
          m_DB[m_CSpecies.id].setFlag(Species::SEL5);
          break;
      case KEY_percent:
          m_DB[m_CSpecies.id].unsetFlag(Species::SEL5);
          break;
      case KEY_comma:
          m_Unit *= 0.5;
          cout << "unit is now " << m_Unit << " mm" << endl;
          break;
      case KEY_period:
          m_Unit *= 2;
          cout << "unit is now " << m_Unit << " mm" << endl;
          break;
      default:
          return FXMainWindow::onKeyPress(obj,sel,ptr);
          break;
  }
  return 1;
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
  _x=m_Data.getDim1Size();
  _y=m_Data.getDim2Size();
  mx=event->win_x;
  my=event->win_y;
#ifdef WIN32
  mx-=glcanvas->getX();
  my-=glcanvas->getY();
#endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  if( mx < _x && my < _y){
      cout << "doing nothing at " << mx << " " << my << endl;
  }
  return 1;
}

long ImageWindow::onRightMouseClick(FXObject*,FXSelector,void* ptr){

  FXEvent		*event=(FXEvent*)ptr;
  FXint			_x, _y, mx, my;

  Point pofs(glcanvas->getX(), glcanvas->getY());
  _x=m_Data.getDim1Size();
  _y=m_Data.getDim2Size();
  mx=event->win_x;
  my=event->win_y;
#ifdef WIN32
  mx-=glcanvas->getX();
  my-=glcanvas->getY();
#endif
  mx=(int)(mx/m_XYScale);
  my=(int)(my/m_XYScale);
  if( mx < _x && my < _y){
      cout << "doing nothing at " << mx << " " << my << endl;
      static int lastclick = -1;
      if(lastclick>0) {
          m_DB[m_CSpecies.id]["scale"] = (FXString().format("%i",int(abs(lastclick-mx)/m_Unit))).text();
          cout << m_DB[m_CSpecies.id] << " for " << m_Unit << " mm" << endl;
          lastclick=-1;
      } else lastclick = mx;
  }
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

  // Make application
  FXApp application("Deformable Model Segmentation","FoxText");

  // Start app
  application.init(argc,argv);

  // Make window
  ImageWindow *iw = new ImageWindow(&application);
  iw->cmdline(argc,argv);

  // Create the application's windows
  application.create();

  // Run the application
  return application.run();
  }

/* emacs layout configuration
;;; Local Variables: ***
;;; tab-width: 4 ***
;;; End: ***
*/
