/*
 *  Worth noting that both the AHT10 and AHT20 are supported without alteration.
 */

#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_AHTX0.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "AHT10.h"
#include "TelemetrySensor.h"

#include <Adafruit_AHTX0.h>
#include <typeinfo>

AHT10Sensor::AHT10Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_AHT10, "AHT10") {}

int32_t AHT10Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    aht10 = Adafruit_AHTX0();
    status = aht10.begin(nodeTelemetrySensorsMap[sensorType].second, 0, nodeTelemetrySensorsMap[sensorType].first);

    return initI2CSensor();
}

void AHT10Sensor::setup() {}

bool AHT10Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    LOG_DEBUG("AHT10 getMetrics");

    sensors_event_t humidity, temp;
    aht10.getEvent(&humidity, &temp);

    measurement->variant.environment_metrics.has_temperature = true;
    measurement->variant.environment_metrics.has_relative_humidity = true;

    measurement->variant.environment_metrics.temperature = temp.temperature;
    measurement->variant.environment_metrics.relative_humidity = humidity.relative_humidity;

    return true;
}

#endif
