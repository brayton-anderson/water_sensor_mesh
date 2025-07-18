#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<bsec2.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "BME680Sensor.h"
#include "FSCommon.h"
#include "SPILock.h"
#include "TelemetrySensor.h"

BME680Sensor::BME680Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_BME680, "BME680") {}

int32_t BME680Sensor::runTrigger()
{
    if (!bme680.run()) {
        checkStatus("runTrigger");
    }
    return 35;
}

int32_t BME680Sensor::runOnce()
{

    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    if (!bme680.begin(nodeTelemetrySensorsMap[sensorType].first, *nodeTelemetrySensorsMap[sensorType].second))
        checkStatus("begin");

    if (bme680.status == BSEC_OK) {
        status = 1;
        if (!bme680.setConfig(bsec_config)) {
            checkStatus("setConfig");
            status = 0;
        }
        loadState();
        if (!bme680.updateSubscription(sensorList, ARRAY_LEN(sensorList), BSEC_SAMPLE_RATE_LP)) {
            checkStatus("updateSubscription");
            status = 0;
        }
        LOG_INFO("Init sensor: %s with the BSEC Library version %d.%d.%d.%d ", sensorName, bme680.version.major,
                 bme680.version.minor, bme680.version.major_bugfix, bme680.version.minor_bugfix);
    } else {
        status = 0;
    }
    if (status == 0)
        LOG_DEBUG("BME680Sensor::runOnce: bme680.status %d", bme680.status);

    return initI2CSensor();
}

void BME680Sensor::setup() {}

bool BME680Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    if (bme680.getData(BSEC_OUTPUT_RAW_PRESSURE).signal == 0)
        return false;

    measurement->variant.environment_metrics.has_temperature = true;
    measurement->variant.environment_metrics.has_relative_humidity = true;
    measurement->variant.environment_metrics.has_barometric_pressure = true;
    measurement->variant.environment_metrics.has_gas_resistance = true;
    measurement->variant.environment_metrics.has_iaq = true;

    measurement->variant.environment_metrics.temperature = bme680.getData(BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE).signal;
    measurement->variant.environment_metrics.relative_humidity =
        bme680.getData(BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY).signal;
    measurement->variant.environment_metrics.barometric_pressure = bme680.getData(BSEC_OUTPUT_RAW_PRESSURE).signal;
    measurement->variant.environment_metrics.gas_resistance = bme680.getData(BSEC_OUTPUT_RAW_GAS).signal / 1000.0;
    // Check if we need to save state to filesystem (every STATE_SAVE_PERIOD ms)
    measurement->variant.environment_metrics.iaq = bme680.getData(BSEC_OUTPUT_IAQ).signal;
    updateState();
    return true;
}

void BME680Sensor::loadState()
{
#ifdef FSCom
    spiLock->lock();
    auto file = FSCom.open(bsecConfigFileName, FILE_O_READ);
    if (file) {
        file.read((uint8_t *)&bsecState, BSEC_MAX_STATE_BLOB_SIZE);
        file.close();
        bme680.setState(bsecState);
        LOG_INFO("%s state read from %s", sensorName, bsecConfigFileName);
    } else {
        LOG_INFO("No %s state found (File: %s)", sensorName, bsecConfigFileName);
    }
    spiLock->unlock();
#else
    LOG_ERROR("ERROR: Filesystem not implemented");
#endif
}

void BME680Sensor::updateState()
{
#ifdef FSCom
    spiLock->lock();
    bool update = false;
    if (stateUpdateCounter == 0) {
        /* First state update when IAQ accuracy is >= 3 */
        accuracy = bme680.getData(BSEC_OUTPUT_IAQ).accuracy;
        if (accuracy >= 2) {
            LOG_DEBUG("%s state update IAQ accuracy %u >= 2", sensorName, accuracy);
            update = true;
            stateUpdateCounter++;
        } else {
            LOG_DEBUG("%s not updated, IAQ accuracy is %u < 2", sensorName, accuracy);
        }
    } else {
        /* Update every STATE_SAVE_PERIOD minutes */
        if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
            LOG_DEBUG("%s state update every %d minutes", sensorName, STATE_SAVE_PERIOD / 60000);
            update = true;
            stateUpdateCounter++;
        }
    }

    if (update) {
        bme680.getState(bsecState);
        if (FSCom.exists(bsecConfigFileName) && !FSCom.remove(bsecConfigFileName)) {
            LOG_WARN("Can't remove old state file");
        }
        auto file = FSCom.open(bsecConfigFileName, FILE_O_WRITE);
        if (file) {
            LOG_INFO("%s state write to %s", sensorName, bsecConfigFileName);
            file.write((uint8_t *)&bsecState, BSEC_MAX_STATE_BLOB_SIZE);
            file.flush();
            file.close();
        } else {
            LOG_INFO("Can't write %s state (File: %s)", sensorName, bsecConfigFileName);
        }
    }
    spiLock->unlock();
#else
    LOG_ERROR("ERROR: Filesystem not implemented");
#endif
}

void BME680Sensor::checkStatus(const char *functionName)
{
    if (bme680.status < BSEC_OK)
        LOG_ERROR("%s BSEC2 code: %d", functionName, bme680.status);
    else if (bme680.status > BSEC_OK)
        LOG_WARN("%s BSEC2 code: %d", functionName, bme680.status);

    if (bme680.sensor.status < BME68X_OK)
        LOG_ERROR("%s BME68X code: %d", functionName, bme680.sensor.status);
    else if (bme680.sensor.status > BME68X_OK)
        LOG_WARN("%s BME68X code: %d", functionName, bme680.sensor.status);
}

#endif
