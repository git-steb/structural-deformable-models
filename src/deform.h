#ifndef _DEFORM_H_
#define _DEFORM_H_
#include "fxconfig.h"
#include <fx.h>
#include <FXApp.h>
#include <string>
#include "glImage.h"
#include "sensordlg.h"
#include "Brain.h"

/*******************************************************************************/
// Event Handler Object
class ImageWindow : public FXMainWindow {

    // Macro for class hierarchy declarations
    FXDECLARE(ImageWindow)

private:

    FXGLCanvas    *glcanvas;                // glcanvas

    FXTimer         *timer;                     // Timer for spinning box

    FXString        filename;                   // File being viewed
    FXString        savefilename;               // File for results
    FXString      directory;                    // Last visited directory

    Brain               m_Brain;
    GLImage             m_GLScreenImg;
    Image<byte>         m_ScreenImg;
    bool                m_RefreshScreenImg;

    FXGLVisual        *glvisual;                // Visual for OpenGL

    float           m_XYScale;
    bool            m_BrainStarted;
    FXStatusLine    *m_Statusline;
    SensorDialog*       m_SensorDlg;            //!< sensor dialog

protected:
    ImageWindow(){}

public:

    // Message handler
    long onCanvasRepaint(FXObject*,FXSelector,void*);
    long onLeftMouseClick(FXObject*,FXSelector,void*);
    long onRightMouseClick(FXObject*,FXSelector,void*);
    long onMouseMove(FXObject*,FXSelector,void*);
    long onKeyPress(FXObject*,FXSelector,void* ptr);
    long onKeyRelease(FXObject*,FXSelector,void* ptr);
    long onCmdRestore(FXObject*,FXSelector,void*);
    long onLoadImage(FXObject*,FXSelector,void*);
    long onExpose(FXObject*,FXSelector,void*);
    long onConfigure(FXObject*,FXSelector,void*);
    long onCmdOpenGL(FXObject*,FXSelector,void*);
    long onToggleSensorDlg(FXObject*,FXSelector,void*);
    long onTimeout(FXObject*,FXSelector,void*);
    int selectSensor(int sido);

    // Draw scene
    void drawScene();
    void setStatusText(const char* msg, bool append=false);

public:

    // Messages for our class
    enum{
        ID_CANVAS=FXMainWindow::ID_LAST,
        ID_TEST,
        ID_LOAD,
        ID_TIMEOUT,
        ID_OPENGL,
        ID_LAST,
        ID_EDSENS,
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

extern bool setTitle(const std::string& title);
extern bool setStatusText(const std::string& stext);

#endif
