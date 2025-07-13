#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && defined(HAS_RAKPROT)

#ifndef _RAK9154SENSOR_H
#define _RAK9154SENSOR_H 1
#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "CurrentSensor.h"
#include "TelemetrySensor.h"
#include "VoltageSensor.h"

class RAK9154Sensor : public TelemetrySensor, VoltageSensor, CurrentSensor
{
  private:
  protected:
    virtual void setup() override;
    uint32_t lastRead = 0;

  public:
    RAK9154Sensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
    virtual uint16_t getBusVoltageMv() override;
    virtual int16_t getCurrentMa() override;
    int getBusBatteryPercent();
    bool isCharging();
    void setLastRead(uint32_t lastRead);
};
#endif // _RAK9154SENSOR_H
#endif // HAS_RAKPROT