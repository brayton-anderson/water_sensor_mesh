#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<SparkFun_MLX90632_Arduino_Library.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <SparkFun_MLX90632_Arduino_Library.h>

class MLX90632Sensor : public TelemetrySensor
{
  private:
    MLX90632 mlx = MLX90632();

  protected:
    virtual void setup() override;

  public:
    MLX90632Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif