#pragma once

#ifndef _MT_DFROBOTLARKSENSOR_H
#define _MT_DFROBOTLARKSENSOR_H
#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include(<DFRobot_LarkWeatherStation.h>)

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "TelemetrySensor.h"
#include <DFRobot_LarkWeatherStation.h>
#include <string>

class DFRobotLarkSensor : public TelemetrySensor
{
  private:
    DFRobot_LarkWeatherStation_I2C lark = DFRobot_LarkWeatherStation_I2C();

  protected:
    virtual void setup() override;

  public:
    DFRobotLarkSensor();
    virtual int32_t runOnce() override;
    virtual bool getMetrics(water_sensor_mesh_Telemetry *measurement) override;
};

#endif
#endif