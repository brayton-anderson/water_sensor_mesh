#pragma once

#include "MeshModule.h"
#include "Router.h"
#include "SinglePortModule.h"
#include "concurrency/OSThread.h"
#include "configuration.h"
#include <Arduino.h>
#include <functional>

#if (defined(ARCH_ESP32) || defined(ARCH_NRF52) || defined(ARCH_RP2040)) && !defined(CONFIG_IDF_TARGET_ESP32S2) &&               \
    !defined(CONFIG_IDF_TARGET_ESP32C3)

class SerialModule : public StreamAPI, private concurrency::OSThread
{
    bool firstTime = 1;
    unsigned long lastNmeaTime = millis();
    char outbuf[90] = "";

  public:
    SerialModule();

  protected:
    virtual int32_t runOnce() override;

    /// Check the current underlying physical link to see if the client is currently connected
    virtual bool checkIsConnected() override;

  private:
    uint32_t getBaudRate();
    void sendTelemetry(water_sensor_mesh_Telemetry m);
    void processWXSerial();
};

extern SerialModule *serialModule;

/*
 * Radio interface for SerialModule
 *
 */
class SerialModuleRadio : public MeshModule
{
    uint32_t lastRxID = 0;
    char outbuf[90] = "";

  public:
    SerialModuleRadio();

    /**
     * Send our payload into the mesh
     */
    void sendPayload(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

  protected:
    virtual water_sensor_mesh_MeshPacket *allocReply() override;

    /** Called to handle a particular incoming message

    @return ProcessMessage::STOP if you've guaranteed you've handled this message and no other handlers should be considered for
    it
    */
    virtual ProcessMessage handleReceived(const water_sensor_mesh_MeshPacket &mp) override;

    water_sensor_mesh_PortNum ourPortNum;

    virtual bool wantPacket(const water_sensor_mesh_MeshPacket *p) override { return p->decoded.portnum == ourPortNum; }

    water_sensor_mesh_MeshPacket *allocDataPacket()
    {
        // Update our local node info with our position (even if we don't decide to update anyone else)
        water_sensor_mesh_MeshPacket *p = router->allocForSending();
        p->decoded.portnum = ourPortNum;

        return p;
    }
};

extern SerialModuleRadio *serialModuleRadio;

#endif