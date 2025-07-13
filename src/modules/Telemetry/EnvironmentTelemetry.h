#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#pragma once

#ifndef ENVIRONMENTAL_TELEMETRY_MODULE_ENABLE
#define ENVIRONMENTAL_TELEMETRY_MODULE_ENABLE 0
#endif

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "NodeDB.h"
#include "ProtobufModule.h"
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>

class EnvironmentTelemetryModule : private concurrency::OSThread, public ProtobufModule<water_sensor_mesh_Telemetry>
{
    CallbackObserver<EnvironmentTelemetryModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<EnvironmentTelemetryModule, const water_sensor_mesh::Status *>(this,
                                                                                 &EnvironmentTelemetryModule::handleStatusUpdate);

  public:
    EnvironmentTelemetryModule()
        : concurrency::OSThread("EnvironmentTelemetry"),
          ProtobufModule("EnvironmentTelemetry", water_sensor_mesh_PortNum_TELEMETRY_APP, &water_sensor_mesh_Telemetry_msg)
    {
        lastMeasurementPacket = nullptr;
        nodeStatusObserver.observe(&nodeStatus->onNewStatus);
        setIntervalFromNow(10 * 1000);
    }
    virtual bool wantUIFrame() override;
#if !HAS_SCREEN
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
#else
    virtual void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
#endif

  protected:
    /** Called to handle a particular incoming message
    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_Telemetry *p) override;
    virtual int32_t runOnce() override;
    /** Called to get current Environment telemetry data
    @return true if it contains valid data
    */
    bool getEnvironmentTelemetry(water_sensor_mesh_Telemetry *m);
    virtual water_sensor_mesh_MeshPacket *allocReply() override;
    /**
     * Send our Telemetry into the mesh
     */
    bool sendTelemetry(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

    virtual AdminMessageHandleResult handleAdminMessageForModule(const water_sensor_mesh_MeshPacket &mp,
                                                                 water_sensor_mesh_AdminMessage *request,
                                                                 water_sensor_mesh_AdminMessage *response) override;

  private:
    bool firstTime = 1;
    water_sensor_mesh_MeshPacket *lastMeasurementPacket;
    uint32_t sendToPhoneIntervalMs = SECONDS_IN_MINUTE * 1000; // Send to phone every minute
    uint32_t lastSentToMesh = 0;
    uint32_t lastSentToPhone = 0;
    uint32_t sensor_read_error_count = 0;
};

#endif