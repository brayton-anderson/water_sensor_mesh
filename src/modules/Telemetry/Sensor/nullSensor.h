#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR
#pragma once

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"

#include "CurrentSensor.h"
#include "TelemetrySensor.h"
#include "VoltageSensor.h"

class NullSensor : public TelemetrySensor, VoltageSensor, CurrentSensor
{

  protected:
    virtual void setup() override;

  public:
    NullSensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    int32_t runTrigger() { return 0; }

    virtual uint16_t getBusVoltageMv() override;
    virtual int16_t getCurrentMa() override;
};

#endif