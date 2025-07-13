#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_LPS2X.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>

class LPS22HBSensor : public TelemetrySensor
{
  private:
    Adafruit_LPS22 lps22hb;

  protected:
    virtual void setup() override;

  public:
    LPS22HBSensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif