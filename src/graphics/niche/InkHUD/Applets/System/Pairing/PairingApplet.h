#ifdef WATER_SENSOR_MESH_INCLUDE_INKHUD

/*

    Shows the Bluetooth passkey during pairing

*/

#pragma once

#include "configuration.h"

#include "graphics/niche/InkHUD/SystemApplet.h"

#include "main.h"

namespace NicheGraphics::InkHUD
{

class PairingApplet : public SystemApplet
{
  public:
    PairingApplet();

    void onRender() override;
    void onForeground() override;
    void onBackground() override;

    int onBluetoothStatusUpdate(const water_sensor_mesh::Status *status);

  protected:
    // Get notified when status of the Bluetooth connection changes
    CallbackObserver<PairingApplet, const water_sensor_mesh::Status *> bluetoothStatusObserver =
        CallbackObserver<PairingApplet, const water_sensor_mesh::Status *>(this, &PairingApplet::onBluetoothStatusUpdate);

    std::string passkey = ""; // Passkey. Six digits, possibly with leading zeros
};

} // namespace NicheGraphics::InkHUD

#endif