#pragma once
#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<ClosedCube_OPT3001.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <ClosedCube_OPT3001.h>

class OPT3001Sensor : public TelemetrySensor
{
  private:
    ClosedCube_OPT3001 opt3001;

  protected:
    virtual void setup() override;

  public:
    OPT3001Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif