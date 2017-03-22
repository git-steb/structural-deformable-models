#ifndef _SENSORCOLL_H_
#define _SENSORCOLL_H_

#include <iostream>
#include <map>
#include "ParseFile.h"
#include "Model.h"

class Sensor;

class SensorCollection : public std::map<std::string,sensor_ptr> {
public:
    SensorCollection();
    ~SensorCollection();

    sensor_ptr addSensor(const std::string& key, sensor_ptr s);
    sensor_ptr addSensor(sensor_ptr s);
    sensor_ptr readSensor(ParseFile& is);
    sensor_ptr getSensor(const std::string& id);
    SensorCollection& merge(SensorCollection& rhs);
    bool isPrinted( const std::string& id ) const;
    void setPrinted( const std::string& id );
    void clearPrintList(bool skipdefaults=true);
    friend std::ostream& operator<<(std::ostream& os,
                                    const SensorCollection& sc);
    void refModel(Model* model) const;
    void unrefModel(Model* model) const;
    void updateModels() const;

    void selectSensor(const std::string& name)
    { m_SelSensor = name; }
    const std::string& getSelectedSensor() const
    { return m_SelSensor; }
protected:
    std::set<std::string>       m_Printlist;
    mutable std::set<Model*>    m_Models; //!< references to attached geometry
    std::string                 m_SelSensor;
};

#endif
