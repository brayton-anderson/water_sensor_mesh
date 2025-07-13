#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#pragma once

class CurrentSensor
{
  public:
    virtual int16_t getCurrentMa() = 0;
};

#endif