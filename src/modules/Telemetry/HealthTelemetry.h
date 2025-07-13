#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && !WATER_SENSOR_MESH_EXCLUDE_HEALTH_TELEMETRY && !defined(ARCH_PORTDUINO)

#pragma once
#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "NodeDB.h"
#include "ProtobufModule.h"
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>

class HealthTelemetryModule : private concurrency::OSThread, public ProtobufModule<water_sensor_mesh_Telemetry>
{
    CallbackObserver<HealthTelemetryModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<HealthTelemetryModule, const water_sensor_mesh::Status *>(this, &HealthTelemetryModule::handleStatusUpdate);

  public:
    HealthTelemetryModule()
        : concurrency::OSThread("HealthTelemetry"),
          ProtobufModule("HealthTelemetry", water_sensor_mesh_PortNum_TELEMETRY_APP, &water_sensor_mesh_Telemetry_msg)
    {
        lastMeasurementPacket = nullptr;
        nodeStatusObserver.observe(&nodeStatus->onNewStatus);
        setIntervalFromNow(10 * 1000);
    }

#if !HAS_SCREEN
    void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y);
#else
    virtual void drawFrame(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y) override;
#endif

    virtual bool wantUIFrame() override;

  protected:
    /** Called to handle a particular incoming message
    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_Telemetry *p) override;
    virtual int32_t runOnce() override;
    /** Called to get current Health telemetry data
    @return true if it contains valid data
    */
    bool getHealthTelemetry(water_sensor_mesh_Telemetry *m);
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