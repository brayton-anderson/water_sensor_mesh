#pragma once

#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<RAK12035_SoilMoisture.h>) && defined(RAK_4631)
#ifndef _MT_RAK12035VBSENSOR_H
#define _MT_RAK12035VBSENSOR_H
#endif

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "RAK12035_SoilMoisture.h"
#include "TelemetrySensor.h"
#include <Arduino.h>

class RAK12035Sensor : public TelemetrySensor
{
  private:
    RAK12035 sensor;

  protected:
    virtual void setup() override;

  public:
    RAK12035Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};
#endif
