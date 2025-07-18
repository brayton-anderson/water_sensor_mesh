#include "configuration.h"

#if HAS_TELEMETRY && !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include("INA226.h")

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "INA226.h"
#include "INA226Sensor.h"
#include "TelemetrySensor.h"

INA226Sensor::INA226Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_INA226, "INA226") {}

int32_t INA226Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }

    begin(nodeTelemetrySensorsMap[sensorType].second, nodeTelemetrySensorsMap[sensorType].first);

    if (!status) {
        status = ina226.begin();
    }
    return initI2CSensor();
}

void INA226Sensor::setup() {}

void INA226Sensor::begin(TwoWire *wire, uint8_t addr)
{
    _wire = wire;
    _addr = addr;
    ina226 = INA226(_addr, _wire);
    _wire->begin();
}

bool INA226Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    measurement->variant.environment_metrics.has_voltage = true;
    measurement->variant.environment_metrics.has_current = true;

    // mV conversion to V
    measurement->variant.environment_metrics.voltage = ina226.getBusVoltage();
    measurement->variant.environment_metrics.current = ina226.getCurrent_mA();
    return true;
}

uint16_t INA226Sensor::getBusVoltageMv()
{
    return lround(ina226.getBusVoltage() * 1000);
}

int16_t INA226Sensor::getCurrentMa()
{
    return lround(ina226.getCurrent_mA());
}

#endif