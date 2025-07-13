#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_VEML7700.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_VEML7700.h>

class VEML7700Sensor : public TelemetrySensor
{
  private:
    const float MAX_RES = 0.0036;
    const float GAIN_MAX = 2;
    const float IT_MAX = 800;
    Adafruit_VEML7700 veml7700;
    float computeLux(uint16_t rawALS, bool corrected);
    float getResolution(void);

  protected:
    virtual void setup() override;

  public:
    VEML7700Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};
#endif