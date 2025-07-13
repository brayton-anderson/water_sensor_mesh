#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<Adafruit_TSL2591.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TSL2591Sensor.h"
#include "TelemetrySensor.h"
#include <Adafruit_TSL2591.h>
#include <typeinfo>

TSL2591Sensor::TSL2591Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_TSL25911FN, "TSL2591") {}

int32_t TSL2591Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    status = tsl.begin(nodeTelemetrySensorsMap[sensorType].second);

    return initI2CSensor();
}

void TSL2591Sensor::setup()
{
    tsl.setGain(TSL2591_GAIN_LOW); // 1x gain
    tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);
}

bool TSL2591Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    measurement->variant.environment_metrics.has_lux = true;
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;

    measurement->variant.environment_metrics.lux = tsl.calculateLux(full, ir);
    LOG_INFO("Lux: %f", measurement->variant.environment_metrics.lux);

    return true;
}

#endif
