#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && !WATER_SENSOR_MESH_EXCLUDE_HEALTH_TELEMETRY && __has_include(<MAX30105.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <MAX30105.h>

#define MAX30102_BUFFER_LEN 100

class MAX30102Sensor : public TelemetrySensor
{
  private:
    MAX30105 max30102 = MAX30105();
    uint32_t _speed = 200000UL;

  protected:
    virtual void setup() override;

  public:
    MAX30102Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif