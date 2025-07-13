#include "configuration.h"

#if HAS_TELEMETRY && !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_INA219.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "CurrentSensor.h"
#include "TelemetrySensor.h"
#include "VoltageSensor.h"
#include <Adafruit_INA219.h>

class INA219Sensor : public TelemetrySensor, VoltageSensor, CurrentSensor
{
  private:
    Adafruit_INA219 ina219;

  protected:
    virtual void setup() override;

  public:
    INA219Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    virtual uint16_t getBusVoltageMv() override;
    virtual int16_t getCurrentMa() override;
};

#endif