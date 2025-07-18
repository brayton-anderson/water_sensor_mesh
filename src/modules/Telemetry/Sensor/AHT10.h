/*
 *  Worth noting that both the AHT10 and AHT20 are supported without alteration.
 */

#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_AHTX0.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_AHTX0.h>

class AHT10Sensor : public TelemetrySensor
{
  private:
    Adafruit_AHTX0 aht10;

  protected:
    virtual void setup() override;

  public:
    AHT10Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif
