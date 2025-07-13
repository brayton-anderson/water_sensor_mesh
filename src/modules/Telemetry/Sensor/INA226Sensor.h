#include "configuration.h"

#if HAS_TELEMETRY && !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "CurrentSensor.h"
#include "TelemetrySensor.h"
#include "VoltageSensor.h"
#include <INA226.h>

class INA226Sensor : public TelemetrySensor, VoltageSensor, CurrentSensor
{
  private:
    uint8_t _addr = INA_ADDR;
    TwoWire *_wire = &Wire;
    INA226 ina226 = INA226(_addr, _wire);

  protected:
    virtual void setup() override;
    void begin(TwoWire *wire = &Wire, uint8_t addr = INA_ADDR);

  public:
    INA226Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    virtual uint16_t getBusVoltageMv() override;
    virtual int16_t getCurrentMa() override;
};

#endif