/*
 *  Support for the ClimateGuard RadSens Dosimeter
 *  A fun and educational sensor for Water_Sensor_Mesh; not for safety critical applications.
 */
#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "CGRadSensSensor.h"
#include "TelemetrySensor.h"
#include <Wire.h>
#include <typeinfo>

CGRadSensSensor::CGRadSensSensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_RADSENS, "RadSens") {}

int32_t CGRadSensSensor::runOnce()
{
    // Initialize the sensor following the same pattern as RCWL9620Sensor
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }

    status = true;
    begin(nodeTelemetrySensorsMap[sensorType].second, nodeTelemetrySensorsMap[sensorType].first);

    return initI2CSensor();
}

void CGRadSensSensor::setup() {}

void CGRadSensSensor::begin(TwoWire *wire, uint8_t addr)
{
    // Store the Wire and address to the sensor following the same pattern as RCWL9620Sensor
    _wire = wire;
    _addr = addr;
    _wire->begin();
}

float CGRadSensSensor::getStaticRadiation()
{
    // Read a register, following the same pattern as the RCWL9620Sensor
    _wire->beginTransmission(_addr); // Transfer data to addr.
    _wire->write(0x06);              // Radiation intensity (static period T = 500 sec)
    if (_wire->endTransmission() == 0) {
        if (_wire->requestFrom(_addr, (uint8_t)3)) {
            ; // Request 3 bytes
            uint32_t data = _wire->read();
            data <<= 8;
            data |= _wire->read();
            data <<= 8;
            data |= _wire->read();

            // As per the data sheet for the RadSens
            // Register 0x06 contains the reading in 0.1 * μR / h
            float microRadPerHr = float(data) / 10.0;
            return microRadPerHr;
        }
    }
    return -1.0;
}

bool CGRadSensSensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    // Store the meansurement in the the appropriate fields of the protobuf
    measurement->variant.environment_metrics.has_radiation = true;

    LOG_DEBUG("CGRADSENS getMetrics");
    measurement->variant.environment_metrics.radiation = getStaticRadiation();

    return true;
}
#endif