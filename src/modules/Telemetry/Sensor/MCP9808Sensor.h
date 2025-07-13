#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_MCP9808.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Adafruit_MCP9808.h>

class MCP9808Sensor : public TelemetrySensor
{
  private:
    Adafruit_MCP9808 mcp9808;

  protected:
    virtual void setup() override;

  public:
    MCP9808Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif