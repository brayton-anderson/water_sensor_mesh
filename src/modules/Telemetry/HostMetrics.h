#pragma once
#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "ProtobufModule.h"

class HostMetricsModule : private concurrency::OSThread, public ProtobufModule<water_sensor_mesh_Telemetry>
{
    CallbackObserver<HostMetricsModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<HostMetricsModule, const water_sensor_mesh::Status *>(this, &HostMetricsModule::handleStatusUpdate);

  public:
    HostMetricsModule()
        : concurrency::OSThread("HostMetrics"),
          ProtobufModule("HostMetrics", water_sensor_mesh_PortNum_TELEMETRY_APP, &water_sensor_mesh_Telemetry_msg)
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
    // virtual water_sensor_mesh_MeshPacket *allocReply() override;
    virtual int32_t runOnce() override;
    /**
     * Send our Telemetry into the mesh
     */
    bool sendMetrics();

  private:
    water_sensor_mesh_Telemetry getHostMetrics();

    uint32_t lastSentToMesh = 0;
    uint32_t uptimeWrapCount;
    uint32_t uptimeLastMs;
};