#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_MCP9808.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "MCP9808Sensor.h"
#include "TelemetrySensor.h"
#include <Adafruit_MCP9808.h>

MCP9808Sensor::MCP9808Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_MCP9808, "MCP9808") {}

int32_t MCP9808Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    status = mcp9808.begin(nodeTelemetrySensorsMap[sensorType].first, nodeTelemetrySensorsMap[sensorType].second);
    return initI2CSensor();
}

void MCP9808Sensor::setup()
{
    mcp9808.setResolution(2);
}

bool MCP9808Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    measurement->variant.environment_metrics.has_temperature = true;

    LOG_DEBUG("MCP9808 getMetrics");
    measurement->variant.environment_metrics.temperature = mcp9808.readTempC();
    return true;
}

#endif