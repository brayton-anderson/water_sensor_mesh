#pragma once
#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_PCT2075.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_PCT2075.h>

class PCT2075Sensor : public TelemetrySensor
{
  private:
    Adafruit_PCT2075 pct2075;

  protected:
    virtual void setup() override;

  public:
    PCT2075Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif
