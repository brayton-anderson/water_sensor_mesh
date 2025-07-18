/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9.1 */

#ifndef PB_WATER_SENSOR_MESH_WATER_SENSOR_MESH_LOCALONLY_PB_H_INCLUDED
#define PB_WATER_SENSOR_MESH_WATER_SENSOR_MESH_LOCALONLY_PB_H_INCLUDED
#include <pb.h>
#include "water_sensor_mesh/config.pb.h"
#include "water_sensor_mesh/module_config.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _water_sensor_mesh_LocalConfig {
    /* The part of the config that is specific to the Device */
    bool has_device;
    water_sensor_mesh_Config_DeviceConfig device;
    /* The part of the config that is specific to the GPS Position */
    bool has_position;
    water_sensor_mesh_Config_PositionConfig position;
    /* The part of the config that is specific to the Power settings */
    bool has_power;
    water_sensor_mesh_Config_PowerConfig power;
    /* The part of the config that is specific to the Wifi Settings */
    bool has_network;
    water_sensor_mesh_Config_NetworkConfig network;
    /* The part of the config that is specific to the Display */
    bool has_display;
    water_sensor_mesh_Config_DisplayConfig display;
    /* The part of the config that is specific to the Lora Radio */
    bool has_lora;
    water_sensor_mesh_Config_LoRaConfig lora;
    /* The part of the config that is specific to the Bluetooth settings */
    bool has_bluetooth;
    water_sensor_mesh_Config_BluetoothConfig bluetooth;
    /* A version integer used to invalidate old save files when we make
 incompatible changes This integer is set at build time and is private to
 NodeDB.cpp in the device code. */
    uint32_t version;
    /* The part of the config that is specific to Security settings */
    bool has_security;
    water_sensor_mesh_Config_SecurityConfig security;
} water_sensor_mesh_LocalConfig;

typedef struct _water_sensor_mesh_LocalModuleConfig {
    /* The part of the config that is specific to the MQTT module */
    bool has_mqtt;
    water_sensor_mesh_ModuleConfig_MQTTConfig mqtt;
    /* The part of the config that is specific to the Serial module */
    bool has_serial;
    water_sensor_mesh_ModuleConfig_SerialConfig serial;
    /* The part of the config that is specific to the ExternalNotification module */
    bool has_external_notification;
    water_sensor_mesh_ModuleConfig_ExternalNotificationConfig external_notification;
    /* The part of the config that is specific to the Store & Forward module */
    bool has_store_forward;
    water_sensor_mesh_ModuleConfig_StoreForwardConfig store_forward;
    /* The part of the config that is specific to the RangeTest module */
    bool has_range_test;
    water_sensor_mesh_ModuleConfig_RangeTestConfig range_test;
    /* The part of the config that is specific to the Telemetry module */
    bool has_telemetry;
    water_sensor_mesh_ModuleConfig_TelemetryConfig telemetry;
    /* The part of the config that is specific to the Canned Message module */
    bool has_canned_message;
    water_sensor_mesh_ModuleConfig_CannedMessageConfig canned_message;
    /* A version integer used to invalidate old save files when we make
 incompatible changes This integer is set at build time and is private to
 NodeDB.cpp in the device code. */
    uint32_t version;
    /* The part of the config that is specific to the Audio module */
    bool has_audio;
    water_sensor_mesh_ModuleConfig_AudioConfig audio;
    /* The part of the config that is specific to the Remote Hardware module */
    bool has_remote_hardware;
    water_sensor_mesh_ModuleConfig_RemoteHardwareConfig remote_hardware;
    /* The part of the config that is specific to the Neighbor Info module */
    bool has_neighbor_info;
    water_sensor_mesh_ModuleConfig_NeighborInfoConfig neighbor_info;
    /* The part of the config that is specific to the Ambient Lighting module */
    bool has_ambient_lighting;
    water_sensor_mesh_ModuleConfig_AmbientLightingConfig ambient_lighting;
    /* The part of the config that is specific to the Detection Sensor module */
    bool has_detection_sensor;
    water_sensor_mesh_ModuleConfig_DetectionSensorConfig detection_sensor;
    /* Paxcounter Config */
    bool has_paxcounter;
    water_sensor_mesh_ModuleConfig_PaxcounterConfig paxcounter;
} water_sensor_mesh_LocalModuleConfig;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define water_sensor_mesh_LocalConfig_init_default      {false, water_sensor_mesh_Config_DeviceConfig_init_default, false, water_sensor_mesh_Config_PositionConfig_init_default, false, water_sensor_mesh_Config_PowerConfig_init_default, false, water_sensor_mesh_Config_NetworkConfig_init_default, false, water_sensor_mesh_Config_DisplayConfig_init_default, false, water_sensor_mesh_Config_LoRaConfig_init_default, false, water_sensor_mesh_Config_BluetoothConfig_init_default, 0, false, water_sensor_mesh_Config_SecurityConfig_init_default}
#define water_sensor_mesh_LocalModuleConfig_init_default {false, water_sensor_mesh_ModuleConfig_MQTTConfig_init_default, false, water_sensor_mesh_ModuleConfig_SerialConfig_init_default, false, water_sensor_mesh_ModuleConfig_ExternalNotificationConfig_init_default, false, water_sensor_mesh_ModuleConfig_StoreForwardConfig_init_default, false, water_sensor_mesh_ModuleConfig_RangeTestConfig_init_default, false, water_sensor_mesh_ModuleConfig_TelemetryConfig_init_default, false, water_sensor_mesh_ModuleConfig_CannedMessageConfig_init_default, 0, false, water_sensor_mesh_ModuleConfig_AudioConfig_init_default, false, water_sensor_mesh_ModuleConfig_RemoteHardwareConfig_init_default, false, water_sensor_mesh_ModuleConfig_NeighborInfoConfig_init_default, false, water_sensor_mesh_ModuleConfig_AmbientLightingConfig_init_default, false, water_sensor_mesh_ModuleConfig_DetectionSensorConfig_init_default, false, water_sensor_mesh_ModuleConfig_PaxcounterConfig_init_default}
#define water_sensor_mesh_LocalConfig_init_zero         {false, water_sensor_mesh_Config_DeviceConfig_init_zero, false, water_sensor_mesh_Config_PositionConfig_init_zero, false, water_sensor_mesh_Config_PowerConfig_init_zero, false, water_sensor_mesh_Config_NetworkConfig_init_zero, false, water_sensor_mesh_Config_DisplayConfig_init_zero, false, water_sensor_mesh_Config_LoRaConfig_init_zero, false, water_sensor_mesh_Config_BluetoothConfig_init_zero, 0, false, water_sensor_mesh_Config_SecurityConfig_init_zero}
#define water_sensor_mesh_LocalModuleConfig_init_zero   {false, water_sensor_mesh_ModuleConfig_MQTTConfig_init_zero, false, water_sensor_mesh_ModuleConfig_SerialConfig_init_zero, false, water_sensor_mesh_ModuleConfig_ExternalNotificationConfig_init_zero, false, water_sensor_mesh_ModuleConfig_StoreForwardConfig_init_zero, false, water_sensor_mesh_ModuleConfig_RangeTestConfig_init_zero, false, water_sensor_mesh_ModuleConfig_TelemetryConfig_init_zero, false, water_sensor_mesh_ModuleConfig_CannedMessageConfig_init_zero, 0, false, water_sensor_mesh_ModuleConfig_AudioConfig_init_zero, false, water_sensor_mesh_ModuleConfig_RemoteHardwareConfig_init_zero, false, water_sensor_mesh_ModuleConfig_NeighborInfoConfig_init_zero, false, water_sensor_mesh_ModuleConfig_AmbientLightingConfig_init_zero, false, water_sensor_mesh_ModuleConfig_DetectionSensorConfig_init_zero, false, water_sensor_mesh_ModuleConfig_PaxcounterConfig_init_zero}

/* Field tags (for use in manual encoding/decoding) */
#define water_sensor_mesh_LocalConfig_device_tag        1
#define water_sensor_mesh_LocalConfig_position_tag      2
#define water_sensor_mesh_LocalConfig_power_tag         3
#define water_sensor_mesh_LocalConfig_network_tag       4
#define water_sensor_mesh_LocalConfig_display_tag       5
#define water_sensor_mesh_LocalConfig_lora_tag          6
#define water_sensor_mesh_LocalConfig_bluetooth_tag     7
#define water_sensor_mesh_LocalConfig_version_tag       8
#define water_sensor_mesh_LocalConfig_security_tag      9
#define water_sensor_mesh_LocalModuleConfig_mqtt_tag    1
#define water_sensor_mesh_LocalModuleConfig_serial_tag  2
#define water_sensor_mesh_LocalModuleConfig_external_notification_tag 3
#define water_sensor_mesh_LocalModuleConfig_store_forward_tag 4
#define water_sensor_mesh_LocalModuleConfig_range_test_tag 5
#define water_sensor_mesh_LocalModuleConfig_telemetry_tag 6
#define water_sensor_mesh_LocalModuleConfig_canned_message_tag 7
#define water_sensor_mesh_LocalModuleConfig_version_tag 8
#define water_sensor_mesh_LocalModuleConfig_audio_tag   9
#define water_sensor_mesh_LocalModuleConfig_remote_hardware_tag 10
#define water_sensor_mesh_LocalModuleConfig_neighbor_info_tag 11
#define water_sensor_mesh_LocalModuleConfig_ambient_lighting_tag 12
#define water_sensor_mesh_LocalModuleConfig_detection_sensor_tag 13
#define water_sensor_mesh_LocalModuleConfig_paxcounter_tag 14

/* Struct field encoding specification for nanopb */
#define water_sensor_mesh_LocalConfig_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  device,            1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  position,          2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  power,             3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  network,           4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  display,           5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  lora,              6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  bluetooth,         7) \
X(a, STATIC,   SINGULAR, UINT32,   version,           8) \
X(a, STATIC,   OPTIONAL, MESSAGE,  security,          9)
#define water_sensor_mesh_LocalConfig_CALLBACK NULL
#define water_sensor_mesh_LocalConfig_DEFAULT NULL
#define water_sensor_mesh_LocalConfig_device_MSGTYPE water_sensor_mesh_Config_DeviceConfig
#define water_sensor_mesh_LocalConfig_position_MSGTYPE water_sensor_mesh_Config_PositionConfig
#define water_sensor_mesh_LocalConfig_power_MSGTYPE water_sensor_mesh_Config_PowerConfig
#define water_sensor_mesh_LocalConfig_network_MSGTYPE water_sensor_mesh_Config_NetworkConfig
#define water_sensor_mesh_LocalConfig_display_MSGTYPE water_sensor_mesh_Config_DisplayConfig
#define water_sensor_mesh_LocalConfig_lora_MSGTYPE water_sensor_mesh_Config_LoRaConfig
#define water_sensor_mesh_LocalConfig_bluetooth_MSGTYPE water_sensor_mesh_Config_BluetoothConfig
#define water_sensor_mesh_LocalConfig_security_MSGTYPE water_sensor_mesh_Config_SecurityConfig

#define water_sensor_mesh_LocalModuleConfig_FIELDLIST(X, a) \
X(a, STATIC,   OPTIONAL, MESSAGE,  mqtt,              1) \
X(a, STATIC,   OPTIONAL, MESSAGE,  serial,            2) \
X(a, STATIC,   OPTIONAL, MESSAGE,  external_notification,   3) \
X(a, STATIC,   OPTIONAL, MESSAGE,  store_forward,     4) \
X(a, STATIC,   OPTIONAL, MESSAGE,  range_test,        5) \
X(a, STATIC,   OPTIONAL, MESSAGE,  telemetry,         6) \
X(a, STATIC,   OPTIONAL, MESSAGE,  canned_message,    7) \
X(a, STATIC,   SINGULAR, UINT32,   version,           8) \
X(a, STATIC,   OPTIONAL, MESSAGE,  audio,             9) \
X(a, STATIC,   OPTIONAL, MESSAGE,  remote_hardware,  10) \
X(a, STATIC,   OPTIONAL, MESSAGE,  neighbor_info,    11) \
X(a, STATIC,   OPTIONAL, MESSAGE,  ambient_lighting,  12) \
X(a, STATIC,   OPTIONAL, MESSAGE,  detection_sensor,  13) \
X(a, STATIC,   OPTIONAL, MESSAGE,  paxcounter,       14)
#define water_sensor_mesh_LocalModuleConfig_CALLBACK NULL
#define water_sensor_mesh_LocalModuleConfig_DEFAULT NULL
#define water_sensor_mesh_LocalModuleConfig_mqtt_MSGTYPE water_sensor_mesh_ModuleConfig_MQTTConfig
#define water_sensor_mesh_LocalModuleConfig_serial_MSGTYPE water_sensor_mesh_ModuleConfig_SerialConfig
#define water_sensor_mesh_LocalModuleConfig_external_notification_MSGTYPE water_sensor_mesh_ModuleConfig_ExternalNotificationConfig
#define water_sensor_mesh_LocalModuleConfig_store_forward_MSGTYPE water_sensor_mesh_ModuleConfig_StoreForwardConfig
#define water_sensor_mesh_LocalModuleConfig_range_test_MSGTYPE water_sensor_mesh_ModuleConfig_RangeTestConfig
#define water_sensor_mesh_LocalModuleConfig_telemetry_MSGTYPE water_sensor_mesh_ModuleConfig_TelemetryConfig
#define water_sensor_mesh_LocalModuleConfig_canned_message_MSGTYPE water_sensor_mesh_ModuleConfig_CannedMessageConfig
#define water_sensor_mesh_LocalModuleConfig_audio_MSGTYPE water_sensor_mesh_ModuleConfig_AudioConfig
#define water_sensor_mesh_LocalModuleConfig_remote_hardware_MSGTYPE water_sensor_mesh_ModuleConfig_RemoteHardwareConfig
#define water_sensor_mesh_LocalModuleConfig_neighbor_info_MSGTYPE water_sensor_mesh_ModuleConfig_NeighborInfoConfig
#define water_sensor_mesh_LocalModuleConfig_ambient_lighting_MSGTYPE water_sensor_mesh_ModuleConfig_AmbientLightingConfig
#define water_sensor_mesh_LocalModuleConfig_detection_sensor_MSGTYPE water_sensor_mesh_ModuleConfig_DetectionSensorConfig
#define water_sensor_mesh_LocalModuleConfig_paxcounter_MSGTYPE water_sensor_mesh_ModuleConfig_PaxcounterConfig

extern const pb_msgdesc_t water_sensor_mesh_LocalConfig_msg;
extern const pb_msgdesc_t water_sensor_mesh_LocalModuleConfig_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define water_sensor_mesh_LocalConfig_fields &water_sensor_mesh_LocalConfig_msg
#define water_sensor_mesh_LocalModuleConfig_fields &water_sensor_mesh_LocalModuleConfig_msg

/* Maximum encoded size of messages (where known) */
#define WATER_SENSOR_MESH_WATER_SENSOR_MESH_LOCALONLY_PB_H_MAX_SIZE water_sensor_mesh_LocalConfig_size
#define water_sensor_mesh_LocalConfig_size              747
#define water_sensor_mesh_LocalModuleConfig_size        669

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
