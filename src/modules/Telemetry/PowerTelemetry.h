#pragma once

#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR

#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "NodeDB.h"
#include "ProtobufModule.h"
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>

class PowerTelemetryModule : private concurrency::OSThread, public ProtobufModule<water_sensor_mesh_Telemetry>
{
    CallbackObserver<PowerTelemetryModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<PowerTelemetryModule, const water_sensor_mesh::Status *>(this, &PowerTelemetryModule::handleStatusUpdate);

  public:
    PowerTelemetryModule()
        : concurrency::OSThread("PowerTelemetry"),
          ProtobufModule("PowerTelemetry", water_sensor_mesh_PortNum_TELEMETRY_APP, &water_sensor_mesh_Telemetry_msg)
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
    /** Called to get current Power telemetry data
    @return true if it contains valid data
    */
    bool getPowerTelemetry(water_sensor_mesh_Telemetry *m);
    virtual water_sensor_mesh_MeshPacket *allocReply() override;
    /**
     * Send our Telemetry into the mesh
     */
    bool sendTelemetry(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

  private:
    bool firstTime = 1;
    water_sensor_mesh_MeshPacket *lastMeasurementPacket;
    uint32_t sendToPhoneIntervalMs = SECONDS_IN_MINUTE * 1000; // Send to phone every minute
    uint32_t lastSentToMesh = 0;
    uint32_t lastSentToPhone = 0;
    uint32_t sensor_read_error_count = 0;
};

#endif