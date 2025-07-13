#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_SHTC3.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_SHTC3.h>

class SHTC3Sensor : public TelemetrySensor
{
  private:
    Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

  protected:
    virtual void setup() override;

  public:
    SHTC3Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif