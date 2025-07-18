/*

E-Ink display driver
    - GDEY0154D67
    - Manufacturer: Goodisplay
    - Size: 1.54 inch
    - Resolution: 200px x 200px
    - Flex connector marking (not a unique identifier): FPC-B001

*/

#pragma once

#ifdef WATER_SENSOR_MESH_INCLUDE_NICHE_GRAPHICS

#include "configuration.h"

#include "./SSD16XX.h"

namespace NicheGraphics::Drivers
{
class GDEY0154D67 : public SSD16XX
{
    // Display properties
  private:
    static constexpr uint32_t width = 200;
    static constexpr uint32_t height = 200;
    static constexpr UpdateTypes supported = (UpdateTypes)(FULL | FAST);

  public:
    GDEY0154D67() : SSD16XX(width, height, supported) {}

  protected:
    void configScanning() override;
    void configWaveform() override;
    void configUpdateSequence() override;
    void detachFromUpdate() override;
};

} // namespace NicheGraphics::Drivers

#endif // WATER_SENSOR_MESH_INCLUDE_NICHE_GRAPHICS