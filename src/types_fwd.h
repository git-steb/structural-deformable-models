/* -*- C++ -*- */
#ifndef __TYPES_FWD_H__
#define __TYPES_FWD_H__

/** \file types_fwd.h
 * Forward declarations of shared pointers.
 */

#include <memory>

class Model;
typedef std::shared_ptr<Model> model_ptr;
typedef std::shared_ptr<const Model> model_cptr;

class Sensor;
typedef std::shared_ptr<Sensor> sensor_ptr;
typedef std::weak_ptr<Sensor> sensor_wptr;
typedef std::shared_ptr<const Sensor> sensor_cptr;
typedef std::weak_ptr<const Sensor> sensor_wcptr;

class Dataset;
typedef std::shared_ptr<Dataset> dataset_ptr;
typedef std::shared_ptr<const Dataset> dataset_cptr;

#endif
