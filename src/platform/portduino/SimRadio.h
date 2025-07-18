#pragma once

#include "MeshPacketQueue.h"
#include "RadioInterface.h"
#include "api/WiFiServerAPI.h"
#include "concurrency/NotifiedWorkerThread.h"

#include <RadioLib.h>

class SimRadio : public RadioInterface, protected concurrency::NotifiedWorkerThread
{
    enum PendingISR { ISR_NONE = 0, ISR_RX, ISR_TX, TRANSMIT_DELAY_COMPLETED };

    MeshPacketQueue txQueue = MeshPacketQueue(MAX_TX_QUEUE);

  public:
    SimRadio();

    /** MeshService needs this to find our active instance
     */
    static SimRadio *instance;

    virtual ErrorCode send(water_sensor_mesh_MeshPacket *p) override;

    /** can we detect a LoRa preamble on the current channel? */
    virtual bool isChannelActive();

    /** are we actively receiving a packet (only called during receiving state)
     *  This method is only public to facilitate debugging.  Do not call.
     */
    virtual bool isActivelyReceiving();

    /** Attempt to cancel a previously sent packet.  Returns true if a packet was found we could cancel */
    virtual bool cancelSending(NodeNum from, PacketId id) override;

    /** Attempt to find a packet in the TxQueue. Returns true if the packet was found. */
    virtual bool findInTxQueue(NodeNum from, PacketId id) override;

    /**
     * Start waiting to receive a message
     *
     * External functions can call this method to wake the device from sleep.
     */
    virtual void startReceive(water_sensor_mesh_MeshPacket *p);

    water_sensor_mesh_QueueStatus getQueueStatus() override;

    // Convert Compressed_msg to normal msg and receive it
    void unpackAndReceive(water_sensor_mesh_MeshPacket &p);

    /**
     * Debugging counts
     */
    uint32_t rxBad = 0, rxGood = 0, txGood = 0, txRelay = 0;

  protected:
    /// are _trying_ to receive a packet currently (note - we might just be waiting for one)
    bool isReceiving = true;

  private:
    void setTransmitDelay();

    /** random timer with certain min. and max. settings */
    void startTransmitTimer(bool withDelay = true);

    /** timer scaled to SNR of to be flooded packet */
    void startTransmitTimerSNR(float snr);

    void handleTransmitInterrupt();
    void handleReceiveInterrupt();

    void onNotify(uint32_t notification);

    // start an immediate transmit
    virtual void startSend(water_sensor_mesh_MeshPacket *txp);

    // derive packet length
    size_t getPacketLength(water_sensor_mesh_MeshPacket *p);

    int16_t readData(uint8_t *str, size_t len);

    water_sensor_mesh_MeshPacket *receivingPacket = nullptr; // The packet we are currently receiving

  protected:
    /** Could we send right now (i.e. either not actively receiving or transmitting)? */
    virtual bool canSendImmediately();

    /**
     * If a send was in progress finish it and return the buffer to the pool */
    void completeSending();
};

extern SimRadio *simRadio;