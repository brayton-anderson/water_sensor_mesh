#ifndef WIFIOTA_H
#define WIFIOTA_H

#include "mesh-pb-constants.h"
#include <Arduino.h>

namespace WiFiOTA
{
void initialize();
bool isUpdated();

void recoverConfig(water_sensor_mesh_Config_NetworkConfig *network);
void saveConfig(water_sensor_mesh_Config_NetworkConfig *network);
bool trySwitchToOTA();
const char *getVersion();
} // namespace WiFiOTA

#endif // WIFIOTA_H
