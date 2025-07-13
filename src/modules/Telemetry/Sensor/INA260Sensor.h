#include "configuration.h"

#if HAS_TELEMETRY && !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_INA260.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include "VoltageSensor.h"
#include <Adafruit_INA260.h>

class INA260Sensor : public TelemetrySensor, VoltageSensor
{
  private:
    Adafruit_INA260 ina260 = Adafruit_INA260();

  protected:
    virtual void setup() override;

  public:
    INA260Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    virtual uint16_t getBusVoltageMv() override;
};

#endif