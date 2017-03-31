#ifndef _SENSORDLG_H_
#define _SENSORDLG_H_

#include "fxconfig.h"
#include <fx.h>
#include <FXDialogBox.h>
#include <FXListBox.h>
#include "vuThread.h"
#include "SensorColl.h"

class ImageWindow;

class SensorDialog : public FXDialogBox {
    // Macro for class hierarchy declarations
    FXDECLARE(SensorDialog);
 public:
    SensorDialog(FXApp *a, ImageWindow* iw, SensorCollection& sc);
    virtual ~SensorDialog();

    long onEdit(FXObject*,FXSelector,void* ptr);
    void updateStrings();
    int getSelectedSensor() const;
    void setGeomMutex(vuMutex* mutex) { m_PGeomMutex = mutex; }

    // Messages for our class
    enum{ ID_CANVAS = FXDialogBox::ID_LAST, ID_SENSEDIT, ID_SENSSEL, ID_LAST };

protected:
SensorDialog() : m_Sensors(*(SensorCollection*)(NULL)) {};

protected:
    SensorCollection&   m_Sensors;
    FXListBox           *m_SList;
    ImageWindow*        m_ImgWin;
    std::vector<int>    m_SIndices;
    vuMutex*            m_PGeomMutex;
};

#endif
