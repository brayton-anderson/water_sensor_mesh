#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_SHT31.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "SHT31Sensor.h"
#include "TelemetrySensor.h"
#include <Adafruit_SHT31.h>

SHT31Sensor::SHT31Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_SHT31, "SHT31") {}

int32_t SHT31Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    sht31 = Adafruit_SHT31(nodeTelemetrySensorsMap[sensorType].second);
    status = sht31.begin(nodeTelemetrySensorsMap[sensorType].first);
    return initI2CSensor();
}

void SHT31Sensor::setup()
{
    // Set up oversampling and filter initialization
}

bool SHT31Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    measurement->variant.environment_metrics.has_temperature = true;
    measurement->variant.environment_metrics.has_relative_humidity = true;
    measurement->variant.environment_metrics.temperature = sht31.readTemperature();
    measurement->variant.environment_metrics.relative_humidity = sht31.readHumidity();

    return true;
}

#endif