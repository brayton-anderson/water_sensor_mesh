#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"

class T1000xSensor : public TelemetrySensor
{
  protected:
    virtual void setup() override;

  public:
    T1000xSensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    virtual float getLux();
    virtual float getTemp();
};

#endif