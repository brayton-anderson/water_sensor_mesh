#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_PCT2075.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "PCT2075Sensor.h"
#include "TelemetrySensor.h"
#include <Adafruit_PCT2075.h>

PCT2075Sensor::PCT2075Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_PCT2075, "PCT2075") {}

int32_t PCT2075Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }

    status = pct2075.begin(nodeTelemetrySensorsMap[sensorType].first, nodeTelemetrySensorsMap[sensorType].second);

    return initI2CSensor();
}

void PCT2075Sensor::setup() {}

bool PCT2075Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    measurement->variant.environment_metrics.has_temperature = true;

    measurement->variant.environment_metrics.temperature = pct2075.getTemperature();

    return true;
}

#endif
