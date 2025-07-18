#include "configuration.h"

#if HAS_TELEMETRY && !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<INA3221.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "CurrentSensor.h"
#include "TelemetrySensor.h"
#include "VoltageSensor.h"
#include <INA3221.h>

#ifndef INA3221_ENV_CH
#define INA3221_ENV_CH INA3221_CH1
#endif

#ifndef INA3221_BAT_CH
#define INA3221_BAT_CH INA3221_CH1
#endif

class INA3221Sensor : public TelemetrySensor, VoltageSensor, CurrentSensor
{
  private:
    INA3221 ina3221 = INA3221(INA3221_ADDR42_SDA);

    // channel to report voltage/current for environment metrics
    static const ina3221_ch_t ENV_CH = INA3221_ENV_CH;

    // channel to report battery voltage for device_battery_ina_address
    static const ina3221_ch_t BAT_CH = INA3221_BAT_CH;

    // get a single measurement for a channel
    struct _INA3221Measurement getMeasurement(ina3221_ch_t ch);

    // get all measurements for all channels
    struct _INA3221Measurements getMeasurements();

    bool getEnvironmentMetrics(water_sensor_mesh_Telemetry *measurement);
    bool getPowerMetrics(water_sensor_mesh_Telemetry *measurement);

  protected:
    void setup() override;

  public:
    INA3221Sensor();
    int32_t runOnce() override;
    bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    virtual uint16_t getBusVoltageMv() override;
    virtual int16_t getCurrentMa() override;
};

struct _INA3221Measurement {
    float voltage;
    float current;
};

struct _INA3221Measurements {
    // INA3221 has 3 channels
    struct _INA3221Measurement measurements[3];
};

#endif