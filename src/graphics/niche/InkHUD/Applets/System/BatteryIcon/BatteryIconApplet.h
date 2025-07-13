#ifdef WATER_SENSOR_MESH_INCLUDE_INKHUD

/*

This applet floats top-left, giving a graphical representation of battery remaining
It should be optional, enabled by the on-screen menu

*/

#pragma once

#include "configuration.h"

#include "graphics/niche/InkHUD/SystemApplet.h"

#include "PowerStatus.h"

namespace NicheGraphics::InkHUD
{

class BatteryIconApplet : public SystemApplet
{
  public:
    BatteryIconApplet();

    void onRender() override;
    int onPowerStatusUpdate(const water_sensor_mesh::Status *status); // Called when new info about battery is available

  private:
    // Get informed when new information about the battery is available (via onPowerStatusUpdate method)
    CallbackObserver<BatteryIconApplet, const water_sensor_mesh::Status *> powerStatusObserver =
        CallbackObserver<BatteryIconApplet, const water_sensor_mesh::Status *>(this, &BatteryIconApplet::onPowerStatusUpdate);

    uint8_t socRounded = 0; // Battery state of charge, rounded to nearest 10%
};

} // namespace NicheGraphics::InkHUD

#endif