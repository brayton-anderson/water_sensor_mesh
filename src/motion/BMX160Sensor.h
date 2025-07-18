#pragma once

#ifndef _BMX160_SENSOR_H_
#define _BMX160_SENSOR_H_

#include "MotionSensor.h"

#if !defined(ARCH_STM32WL) && !WATER_SENSOR_MESH_EXCLUDE_I2C

#if defined(RAK_4631) && !defined(RAK2560) && __has_include(<Rak_BMX160.h>)

#include "Fusion/Fusion.h"
#include <Rak_BMX160.h>

class BMX160Sensor : public MotionSensor
{
  private:
    RAK_BMX160 sensor;
    bool showingScreen = false;
    float highestX = 0, lowestX = 0, highestY = 0, lowestY = 0, highestZ = 0, lowestZ = 0;

  public:
    explicit BMX160Sensor(ScanI2C::FoundDevice foundDevice);
    virtual bool init() override;
    virtual int32_t runOnce() override;
    virtual void calibrate(uint16_t forSeconds) override;
};

#else

// Stub
class BMX160Sensor : public MotionSensor
{
  public:
    explicit BMX160Sensor(ScanI2C::FoundDevice foundDevice);
};

#endif

#endif

#endif