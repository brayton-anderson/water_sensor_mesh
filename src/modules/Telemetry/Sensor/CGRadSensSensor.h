/*
 *  Support for the ClimateGuard RadSens Dosimeter
 *  A fun and educational sensor for Water_Sensor_Mesh; not for safety critical applications.
 */
#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <Wire.h>

class CGRadSensSensor : public TelemetrySensor
{
  private:
    uint8_t _addr = 0x66;
    TwoWire *_wire = &Wire;

  protected:
    virtual void setup() override;
    void begin(TwoWire *wire = &Wire, uint8_t addr = 0x66);
    float getStaticRadiation();

  public:
    CGRadSensSensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif