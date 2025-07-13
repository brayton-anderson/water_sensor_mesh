#include "DisplayFormatters.h"

const char *DisplayFormatters::getModemPresetDisplayName(water_sensor_mesh_Config_LoRaConfig_ModemPreset preset, bool useShortName)
{
    switch (preset) {
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_SHORT_TURBO:
        return useShortName ? "ShortT" : "ShortTurbo";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_SHORT_SLOW:
        return useShortName ? "ShortS" : "ShortSlow";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_SHORT_FAST:
        return useShortName ? "ShortF" : "ShortFast";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_MEDIUM_SLOW:
        return useShortName ? "MedS" : "MediumSlow";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_MEDIUM_FAST:
        return useShortName ? "MedF" : "MediumFast";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_LONG_SLOW:
        return useShortName ? "LongS" : "LongSlow";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_LONG_FAST:
        return useShortName ? "LongF" : "LongFast";
        break;
    case water_sensor_mesh_Config_LoRaConfig_ModemPreset_LONG_MODERATE:
        return useShortName ? "LongM" : "LongMod";
        break;
    default:
        return useShortName ? "Custom" : "Invalid";
        break;
    }
}