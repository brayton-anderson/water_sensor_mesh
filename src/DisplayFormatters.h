#pragma once
#include "NodeDB.h"

class DisplayFormatters
{
  public:
    static const char *getModemPresetDisplayName(water_sensor_mesh_Config_LoRaConfig_ModemPreset preset, bool useShortName);
};
