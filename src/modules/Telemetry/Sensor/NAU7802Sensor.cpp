#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<SparkFun_Qwiic_Scale_NAU7802_Arduino_Library.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "FSCommon.h"
#include "NAU7802Sensor.h"
#include "SPILock.h"
#include "SafeFile.h"
#include "TelemetrySensor.h"
#include <Throttle.h>
#include <pb_decode.h>
#include <pb_encode.h>

water_sensor_mesh_Nau7802Config nau7802config = water_sensor_mesh_Nau7802Config_init_zero;

NAU7802Sensor::NAU7802Sensor() : TelemetrySensor(water_sensor_mesh_TelemetrySensorType_NAU7802, "NAU7802") {}

int32_t NAU7802Sensor::runOnce()
{
    LOG_INFO("Init sensor: %s", sensorName);
    if (!hasSensor()) {
        return DEFAULT_SENSOR_MINIMUM_WAIT_TIME_BETWEEN_READS;
    }
    status = nau7802.begin(*nodeTelemetrySensorsMap[sensorType].second);
    nau7802.setSampleRate(NAU7802_SPS_320);
    if (!loadCalibrationData()) {
        LOG_ERROR("Failed to load calibration data");
    }
    nau7802.calibrateAFE();
    LOG_INFO("Offset: %d, Calibration factor: %.2f", nau7802.getZeroOffset(), nau7802.getCalibrationFactor());
    return initI2CSensor();
}

void NAU7802Sensor::setup() {}

bool NAU7802Sensor::getMetrics(water_sensor_mesh_Telemetry *measurement)
{
    LOG_DEBUG("NAU7802 getMetrics");
    nau7802.powerUp();
    // Wait for the sensor to become ready for one second max
    uint32_t start = millis();
    while (!nau7802.available()) {
        delay(100);
        if (!Throttle::isWithinTimespanMs(start, 1000)) {
            nau7802.powerDown();
            return false;
        }
    }
    measurement->variant.environment_metrics.has_weight = true;
    // Check if we have correct calibration values after powerup
    LOG_DEBUG("Offset: %d, Calibration factor: %.2f", nau7802.getZeroOffset(), nau7802.getCalibrationFactor());
    measurement->variant.environment_metrics.weight = nau7802.getWeight() / 1000; // sample is in kg
    nau7802.powerDown();
    return true;
}

void NAU7802Sensor::calibrate(float weight)
{
    nau7802.calculateCalibrationFactor(weight * 1000, 64); // internal sample is in grams
    if (!saveCalibrationData()) {
        LOG_WARN("Failed to save calibration data");
    }
    LOG_INFO("Offset: %d, Calibration factor: %.2f", nau7802.getZeroOffset(), nau7802.getCalibrationFactor());
}

AdminMessageHandleResult NAU7802Sensor::handleAdminMessage(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_AdminMessage *request,
                                                           water_sensor_mesh_AdminMessage *response)
{
    AdminMessageHandleResult result;

    switch (request->which_payload_variant) {
    case water_sensor_mesh_AdminMessage_set_scale_tag:
        if (request->set_scale == 0) {
            this->tare();
            LOG_DEBUG("Client requested to tare scale");
        } else {
            this->calibrate(request->set_scale);
            LOG_DEBUG("Client requested to calibrate to %d kg", request->set_scale);
        }
        result = AdminMessageHandleResult::HANDLED;
        break;

    default:
        result = AdminMessageHandleResult::NOT_HANDLED;
    }

    return result;
}

void NAU7802Sensor::tare()
{
    nau7802.calculateZeroOffset(64);
    if (!saveCalibrationData()) {
        LOG_WARN("Failed to save calibration data");
    }
    LOG_INFO("Offset: %d, Calibration factor: %.2f", nau7802.getZeroOffset(), nau7802.getCalibrationFactor());
}

bool NAU7802Sensor::saveCalibrationData()
{
    auto file = SafeFile(nau7802ConfigFileName);
    nau7802config.zeroOffset = nau7802.getZeroOffset();
    nau7802config.calibrationFactor = nau7802.getCalibrationFactor();
    bool okay = false;

    LOG_INFO("%s state write to %s", sensorName, nau7802ConfigFileName);
    pb_ostream_t stream = {&writecb, static_cast<Print *>(&file), water_sensor_mesh_Nau7802Config_size};

    if (!pb_encode(&stream, &water_sensor_mesh_Nau7802Config_msg, &nau7802config)) {
        LOG_ERROR("Error: can't encode protobuf %s", PB_GET_ERROR(&stream));
    } else {
        okay = true;
    }
    spiLock->lock();
    okay &= file.close();
    spiLock->unlock();

    return okay;
}

bool NAU7802Sensor::loadCalibrationData()
{
    spiLock->lock();
    auto file = FSCom.open(nau7802ConfigFileName, FILE_O_READ);
    bool okay = false;
    if (file) {
        LOG_INFO("%s state read from %s", sensorName, nau7802ConfigFileName);
        pb_istream_t stream = {&readcb, &file, water_sensor_mesh_Nau7802Config_size};
        if (!pb_decode(&stream, &water_sensor_mesh_Nau7802Config_msg, &nau7802config)) {
            LOG_ERROR("Error: can't decode protobuf %s", PB_GET_ERROR(&stream));
        } else {
            nau7802.setZeroOffset(nau7802config.zeroOffset);
            nau7802.setCalibrationFactor(nau7802config.calibrationFactor);
            okay = true;
        }
        file.close();
    } else {
        LOG_INFO("No %s state found (File: %s)", sensorName, nau7802ConfigFileName);
    }
    spiLock->unlock();
    return okay;
}

#endif