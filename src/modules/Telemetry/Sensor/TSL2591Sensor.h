#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_TSL2591.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_TSL2591.h>

class TSL2591Sensor : public TelemetrySensor
{
  private:
    Adafruit_TSL2591 tsl;

  protected:
    virtual void setup() override;

  public:
    TSL2591Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};
#endif