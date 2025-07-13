#include <sys/types.h>

#pragma once
#include "ProtobufModule.h"
#if HAS_WIFI
#include "mesh/wifi/WiFiAPClient.h"
#endif

/**
 * Datatype passed to Observers by AdminModule, to allow external handling of admin messages
 */
struct AdminModule_ObserverData {
    const water_sensor_mesh_AdminMessage *request;
    water_sensor_mesh_AdminMessage *response;
    AdminMessageHandleResult *result;
};

/**
 * Admin module for admin messages
 */
class AdminModule : public ProtobufModule<water_sensor_mesh_AdminMessage>, public Observable<AdminModule_ObserverData *>
{
  public:
    /** Constructor
     * name is for debugging output
     */
    AdminModule();

  protected:
    /** Called to handle a particular incoming message

    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_AdminMessage *p) override;

  private:
    bool hasOpenEditTransaction = false;

    uint8_t session_passkey[8] = {0};
    uint session_time = 0;

    void saveChanges(int saveWhat, bool shouldReboot = true);

    /**
     * Getters
     */
    void handleGetModuleConfigResponse(const water_sensor_mesh_MeshPacket &req, water_sensor_mesh_AdminMessage *p);
    void handleGetOwner(const water_sensor_mesh_MeshPacket &req);
    void handleGetConfig(const water_sensor_mesh_MeshPacket &req, uint32_t configType);
    void handleGetModuleConfig(const water_sensor_mesh_MeshPacket &req, uint32_t configType);
    void handleGetChannel(const water_sensor_mesh_MeshPacket &req, uint32_t channelIndex);
    void handleGetDeviceMetadata(const water_sensor_mesh_MeshPacket &req);
    void handleGetDeviceConnectionStatus(const water_sensor_mesh_MeshPacket &req);
    void handleGetNodeRemoteHardwarePins(const water_sensor_mesh_MeshPacket &req);
    void handleGetDeviceUIConfig(const water_sensor_mesh_MeshPacket &req);
    /**
     * Setters
     */
    void handleSetOwner(const water_sensor_mesh_User &o);
    void handleSetChannel(const water_sensor_mesh_Channel &cc);
    void handleSetConfig(const water_sensor_mesh_Config &c);
    bool handleSetModuleConfig(const water_sensor_mesh_ModuleConfig &c);
    void handleSetChannel();
    void handleSetHamMode(const water_sensor_mesh_HamParameters &req);
    void handleStoreDeviceUIConfig(const water_sensor_mesh_DeviceUIConfig &uicfg);
    void handleSendInputEvent(const water_sensor_mesh_AdminMessage_InputEvent &inputEvent);
    void reboot(int32_t seconds);

    void setPassKey(water_sensor_mesh_AdminMessage *res);
    bool checkPassKey(water_sensor_mesh_AdminMessage *res);

    bool messageIsResponse(const water_sensor_mesh_AdminMessage *r);
    bool messageIsRequest(const water_sensor_mesh_AdminMessage *r);
    void sendWarning(const char *message);
};

static constexpr const char *licensedModeMessage =
    "Licensed mode activated, removing admin channel and encryption from all channels";

extern AdminModule *adminModule;

void disableBluetooth();