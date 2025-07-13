#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_BMP280.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_BMP280.h>

class BMP280Sensor : public TelemetrySensor
{
  private:
    Adafruit_BMP280 bmp280;

  protected:
    virtual void setup() override;

  public:
    BMP280Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif