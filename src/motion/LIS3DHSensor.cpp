#include "LIS3DHSensor.h"
#include "NodeDB.h"

#if !defined(ARCH_STM32WL) && !WATER_SENSOR_MESH_EXCLUDE_I2C && __has_include(<Adafruit_LIS3DH.h>)

LIS3DHSensor::LIS3DHSensor(ScanI2C::FoundDevice foundDevice) : MotionSensor::MotionSensor(foundDevice) {}

bool LIS3DHSensor::init()
{
    if (sensor.begin(deviceAddress())) {
        sensor.setRange(LIS3DH_RANGE_2_G);
        // Adjust threshold, higher numbers are less sensitive
        sensor.setClick(config.device.double_tap_as_button_press ? 2 : 1, MOTION_SENSOR_CHECK_INTERVAL_MS);
        LOG_DEBUG("LIS3DH init ok");
        return true;
    }
    LOG_DEBUG("LIS3DH init failed");
    return false;
}

int32_t LIS3DHSensor::runOnce()
{
    if (sensor.getClick() > 0) {
        uint8_t click = sensor.getClick();
        if (!config.device.double_tap_as_button_press && config.display.wake_on_tap_or_motion) {
            wakeScreen();
        }

        if (config.device.double_tap_as_button_press && (click & 0x20)) {
            buttonPress();
            return 500;
        }
    }
    return MOTION_SENSOR_CHECK_INTERVAL_MS;
}

#endif
