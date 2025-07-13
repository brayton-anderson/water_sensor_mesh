#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#pragma once
#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "MeshModule.h"
#include "NodeDB.h"
#include <utility>

#if !ARCH_PORTDUINO
class TwoWire;
#endif

#define DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS 1000
extern std::pair<uint8_t, TwoWire *> nodeTelemetrySensorsMap[_water_sensor_mesh_TelemetrySensorType_MAX + 1];

class TelemetrySensor
{
  protected:
    TelemetrySensor(water_sensor_mesh_TelemetrySensorType sensorType, const char *sensorName)
    {
        this->sensorName = sensorName;
        this->sensorType = sensorType;
        this->status = 0;
    }

    const char *sensorName;
    water_sensor_mesh_TelemetrySensorType sensorType = water_sensor_mesh_TelemetrySensorType_SENSOR_UNSET;
    unsigned status;
    bool initialized = false;

    int32_t initI2CSensor()
    {
        if (!status) {
            LOG_WARN("Can't connect to detected %s sensor. Remove from nodeTelemetrySensorsMap", sensorName);
            nodeTelemetrySensorsMap[sensorType].first = 0;
        } else {
            LOG_INFO("Opened %s sensor on i2c bus", sensorName);
            setup();
        }
        initialized = true;
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    virtual void setup() = 0;

  public:
    virtual AdminMessageHandleResult handleAdminMessage(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_AdminMessage *request,
                                                        water_sensor_mesh_AdminMessage *response)
    {
        return AdminMessageHandleResult::NOT_HANDLED;
    }

    bool hasSensor() { return nodeTelemetrySensorsMap[sensorType].first > 0; }

    virtual int32_t runOnce() = 0;
    virtual bool isInitialized() { return initialized; }
    virtual bool isRunning() { return status > 0; }

    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) = 0;
};

#endif