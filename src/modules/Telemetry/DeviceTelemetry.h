#pragma once
#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "NodeDB.h"
#include "ProtobufModule.h"
#include <OLEDDisplay.h>
#include <OLEDDisplayUi.h>

class DeviceTelemetryModule : private concurrency::OSThread, public ProtobufModule<water_sensor_mesh_Telemetry>
{
    CallbackObserver<DeviceTelemetryModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<DeviceTelemetryModule, const water_sensor_mesh::Status *>(this, &DeviceTelemetryModule::handleStatusUpdate);

  public:
    DeviceTelemetryModule()
        : concurrency::OSThread("DeviceTelemetry"),
          ProtobufModule("DeviceTelemetry", water_sensor_mesh_PortNum_TELEMETRY_APP, &water_sensor_mesh_Telemetry_msg)
    {
        uptimeWrapCount = 0;
        uptimeLastMs = millis();
        nodeStatusObserver.observe(&nodeStatus->onNewStatus);
        setIntervalFromNow(setStartDelay()); // Wait until NodeInfo is sent
    }
    virtual bool wantUIFrame() { return false; }

  protected:
    /** Called to handle a particular incoming message
    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_Telemetry *p) override;
    virtual water_sensor_mesh_MeshPacket *allocReply() override;
    virtual int32_t runOnce() override;
    /**
     * Send our Telemetry into the mesh
     */
    bool sendTelemetry(NodeNum dest = NODENUM_BROADCAST, bool phoneOnly = false);

    /**
     * Get the uptime in seconds
     * Loses some accuracy after 49 days, but that's fine
     */
    uint32_t getUptimeSeconds() { return (0xFFFFFFFF / 1000) * uptimeWrapCount + (uptimeLastMs / 1000); }

  private:
    water_sensor_mesh_Telemetry getDeviceTelemetry();
    water_sensor_mesh_Telemetry getLocalStatsTelemetry();

    void sendLocalStatsToPhone();
    uint32_t sendToPhoneIntervalMs = SECONDS_IN_MINUTE * 1000;           // Send to phone every minute
    uint32_t sendStatsToPhoneIntervalMs = 15 * SECONDS_IN_MINUTE * 1000; // Send stats to phone every 15 minutes
    uint32_t lastSentStatsToPhone = 0;
    uint32_t lastSentToMesh = 0;

    void refreshUptime()
    {
        auto now = millis();
        // If we wrapped around (~49 days), increment the wrap count
        if (now < uptimeLastMs)
            uptimeWrapCount++;

        uptimeLastMs = now;
    }

    uint32_t uptimeWrapCount;
    uint32_t uptimeLastMs;
};