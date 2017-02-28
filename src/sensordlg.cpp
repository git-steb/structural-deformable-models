#include <iostream>
#include <sstream>
#include "sensordlg.h"
#include "common.h"
#include <FXInputDialog.h>
#include "deform.h"

using namespace std;

// Message Map
FXDEFMAP(SensorDialog) SensorDialogMap[]={
  //____Message_Type______________ID_______________Message_Handler___
    FXMAPFUNC(SEL_COMMAND, SensorDialog::ID_SENSEDIT, SensorDialog::onEdit),
    FXMAPFUNC(SEL_COMMAND, SensorDialog::ID_SENSSEL, SensorDialog::onEdit)
};

// SensorDialog implementation
FXIMPLEMENT(SensorDialog,FXDialogBox,SensorDialogMap,
            ARRAYNUMBER(SensorDialogMap))
    

SensorDialog::SensorDialog(FXApp *a, ImageWindow* iw, SensorCollection& sc)
    : FXDialogBox (a, "Sensor Editor",DECOR_TITLE|DECOR_BORDER,0,0,400,0), 
      m_Sensors(sc), m_SList(NULL), m_ImgWin(iw), m_PGeomMutex(NULL)
{
    FXHorizontalFrame* contents=new FXHorizontalFrame(
        this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,0,0,0,0, 0,0,0,0);
    m_SList = new FXListBox(contents, this, ID_SENSSEL, 
                            LISTBOX_NORMAL|LAYOUT_FILL_X);
    m_SList->setNumVisible(5);
    m_SList->setWidth(400);
    new FXButton(contents, "&Edit",NULL, this, ID_SENSEDIT);
    new FXButton(contents, "&Hide",NULL, this, ID_HIDE);
    updateStrings();
}

SensorDialog::~SensorDialog()
{
    if(m_SList) { delete m_SList; m_SList = NULL; }
}

long SensorDialog::onEdit(FXObject*,FXSelector sel,void* ptr)
{
    if(sel == MKUINT(ID_SENSSEL,SEL_COMMAND)) {
        vuMutex blah;
        vuLock(m_PGeomMutex ? *m_PGeomMutex : blah);
        if(m_ImgWin) m_ImgWin->selectSensor(getSelectedSensor());
    } else {
        FXInputDialog inpd(this, "Edit Sensor", "Sensor string");
        inpd.setText(m_SList->getItem(m_SList->getCurrentItem()));
        if(inpd.execute()) {
            vuMutex blah;
            vuLock(m_PGeomMutex ? *m_PGeomMutex : blah);
            stringstream ss;
            ss << inpd.getText().text();
            ParseFile pf(ss);
            Sensor* sensor = m_Sensors.readSensor(pf);
            if(sensor) {
                sensor = m_Sensors.addSensor(sensor);
                m_Sensors.updateModels();
                //if(m_ImgWin) m_ImgWin->selectSensor(getSelectedSensor());
                //m_SList->setItemText(m_SList->getCurrentItem(), 
                //                     inpd.getText());
                updateStrings();
            }
        }
    }
    return 1;
}

void SensorDialog::updateStrings()
{
    m_SIndices.clear();
    m_SList->clearItems();
    int sid=0;
    for(SensorCollection::const_iterator s=m_Sensors.begin();
        s!=m_Sensors.end();s++,sid++) 
    {
        if(s->second->getID() != "0"            && // zero sensor
           s->second->getID() != "d0"           &&
           !s->second->getID().empty())
        {
            stringstream ss;
            s->second->print(ss);
            m_SList->appendItem(ss.str().c_str());
            //cout << sid <<": " << ss.str() << endl;
            m_SIndices.push_back(sid);
        }
    }
    m_SList->appendItem("# new sensor #");
    m_SIndices.push_back(sid);
    m_SList->setNumVisible(m_SList->getNumItems());
    
}

int SensorDialog::getSelectedSensor() const {
    return shown() ? m_SIndices[m_SList->getCurrentItem()] : -1;
}
