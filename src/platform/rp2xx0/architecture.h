#pragma once

#define ARCH_RP2040

#ifndef HAS_BUTTON
#define HAS_BUTTON 1
#endif
#ifndef HAS_TELEMETRY
#define HAS_TELEMETRY 1
#endif
#ifndef HAS_SCREEN
#define HAS_SCREEN 1
#endif
#ifndef HAS_WIRE
#define HAS_WIRE 1
#endif
#ifndef HAS_SENSOR
#define HAS_SENSOR 1
#endif
#ifndef HAS_RADIO
#define HAS_RADIO 1
#endif

#if defined(RPI_PICO)
#define HW_VENDOR water_sensor_mesh_HardwareModel_RPI_PICO
#elif defined(RPI_PICO2)
#define HW_VENDOR water_sensor_mesh_HardwareModel_RPI_PICO2
#elif defined(RAK11310)
#define HW_VENDOR water_sensor_mesh_HardwareModel_RAK11310
#elif defined(SENSELORA_RP2040)
#define HW_VENDOR water_sensor_mesh_HardwareModel_SENSELORA_RP2040
#elif defined(RP2040_LORA)
#define HW_VENDOR water_sensor_mesh_HardwareModel_RP2040_LORA
#elif defined(RP2040_FEATHER_RFM95)
#define HW_VENDOR water_sensor_mesh_HardwareModel_RP2040_FEATHER_RFM95
#elif defined(PRIVATE_HW)
#define HW_VENDOR water_sensor_mesh_HardwareModel_PRIVATE_HW
#endif