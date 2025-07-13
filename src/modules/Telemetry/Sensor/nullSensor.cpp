#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include "nullSensor.h"
#include <typeinfo>

NullSensor::NullSensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_SENSOR_UNSET, "nullSensor") {}

int32_t NullSensor::runOnce()
{
    return 0;
}

void NullSensor::setup() {}

bool NullSensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    return false;
}

uint16_t NullSensor::getBusVoltageMv()
{
    return 0;
}

int16_t NullSensor::getCurrentMa()
{
    return 0;
}

#endif