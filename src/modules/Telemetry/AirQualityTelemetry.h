#include "configuration.h"

#if !WATER_SENSOR_MESH_EXCLUDE_ENVIRONMENTAL_SENSOR && __has_include("Adafruit_PM25AQI.h")

#pragma once
#include "../mesh/generated/water_sensor_mesh/telemetry.pb.h"
#include "Adafruit_PM25AQI.h"
#include "NodeDB.h"
#include "ProtobufModule.h"

class AirQualityTelemetryModule : private concurrency::OSThread, public ProtobufModule<water_sensor_mesh_Telemetry>
{
    CallbackObserver<AirQualityTelemetryModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<AirQualityTelemetryModule, const water_sensor_mesh::Status *>(this,
                                                                                &AirQualityTelemetryModule::handleStatusUpdate);

  public:
    AirQualityTelemetryModule()
        : concurrency::OSThread("AirQualityTelemetry"),
          ProtobufModule("AirQualityTelemetry", water_sensor_mesh_PortNum_TELEMETRY_APP, &water_sensor_mesh_Telemetry_msg)
    {
        lastMeasurementPacket = nullptr;
        setIntervalFromNow(10 * 1000);
        aqi = Adafruit_PM25AQI();
        nodeStatusObserver.observe(&nodeStatus->onNewStatus);

#ifdef PMSA003I_ENABLE_PIN
        // the PMSA003I sensor uses about 300mW on its own; support powering it off when it's not actively taking
        // a reading
        state = State::IDLE;
#else
        state = State::ACTIVE;
#endif
    }

  protected:
    /** Called to handle a particular incoming message
    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_Telemetry *p) override;
    virtual int32_t runOnce() override;
    /** Called to get current Air Quality data
    @return true if it contains valid data
    */
    bool getAirQualityTelemetry(water_sensor_mesh_Telemetry *m);
    virtual water_sensor_mesh_MeshPacket *allocReply() override;
    /**
     * Send our Telemetry into the mesh
     */
    bool sendTelemetry(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

  private:
    enum State {
        IDLE = 0,
        ACTIVE = 1,
    };

    State state;
    Adafruit_PM25AQI aqi;
    PM25_AQI_Data data = {0};
    bool firstTime = true;
    water_sensor_mesh_MeshPacket *lastMeasurementPacket;
    uint32_t sendToPhoneIntervalMs = SECONDS_IN_MINUTE * 1000; // Send to phone every minute
    uint32_t lastSentToMesh = 0;
};

#endif