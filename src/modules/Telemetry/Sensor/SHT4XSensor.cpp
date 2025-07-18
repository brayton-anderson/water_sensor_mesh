#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_SHT4x.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "SHT4XSensor.h"
#include "TelemetrySensor.h"
#include <Adafruit_SHT4x.h>

SHT4XSensor::SHT4XSensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_SHT4X, "SHT4X") {}

int32_t SHT4XSensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }

    uint32_t serialNumber = 0;

    sht4x.begin(nodeTelemetrySensorsMap[sensorType].second);

    serialNumber = sht4x.readSerial();
    if (serialNumber != 0) {
        LOG_DEBUG("serialNumber : %x", serialNumber);
        status = 1;
    } else {
        LOG_DEBUG("Error trying to execute readSerial(): ");
        status = 0;
    }

    return initI2CSensor();
}

void SHT4XSensor::setup()
{
    // Set up oversampling and filter initialization
}

bool SHT4XSensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    measurement->variant.environment_metrics.has_temperature = true;
    measurement->variant.environment_metrics.has_relative_humidity = true;

    sensors_event_t humidity, temp;
    sht4x.getEvent(&humidity, &temp);
    measurement->variant.environment_metrics.temperature = temp.temperature;
    measurement->variant.environment_metrics.relative_humidity = humidity.relative_humidity;
    return true;
}

#endif