#pragma once
#include "ProtobufModule.h"
#define MAX_NUM_NEIGHBORS 10 // also defined in NeighborInfo protobuf options

/*
 * Neighborinfo module for sending info on each node's 0-hop neighbors to the mesh
 */
class NeighborInfoModule : public ProtobufModule<water_sensor_mesh_NeighborInfo>, private concurrency::OSThread
{
    CallbackObserver<NeighborInfoModule, const water_sensor_mesh::Status *> nodeStatusObserver =
        CallbackObserver<NeighborInfoModule, const water_sensor_mesh::Status *>(this, &NeighborInfoModule::handleStatusUpdate);

    std::vector<water_sensor_mesh_Neighbor> neighbors;

  public:
    /*
     * Expose the constructor
     */
    NeighborInfoModule();

    /* Reset neighbor info after clearing nodeDB*/
    void resetNeighbors();

  protected:
    /*
     * Called to handle a particular incoming message
     * @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
     */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_NeighborInfo *nb) override;

    /*
     * Collect neighbor info from the nodeDB's history, capping at a maximum number of entries and max time
     * @return the number of entries collected
     */
    uint32_t collectNeighborInfo(water_sensor_mesh_NeighborInfo *neighborInfo);

    /*
      Remove neighbors from the database that we haven't heard from in a while
    */
    void cleanUpNeighbors();

    /* Allocate a new NeighborInfo packet */
    water_sensor_mesh_NeighborInfo *allocateNeighborInfoPacket();

    // Find a neighbor in our DB, create an empty neighbor if missing
    water_sensor_mesh_Neighbor *getOrCreateNeighbor(NodeNum originalSender, NodeNum n, uint32_t node_broadcast_interval_secs, float snr);

    /*
     * Send info on our node's neighbors into the mesh
     */
    void sendNeighborInfo(NodeNum dest = NODENUM_BROADCAST, bool wantReplies = false);

    /* update neighbors with subpacket sniffed from network */
    void updateNeighbors(const water_sensor_mesh_MeshPacket &mp, const water_sensor_mesh_NeighborInfo *np);

    /* update a NeighborInfo packet with our NodeNum as last_sent_by_id */
    void alterReceivedProtobuf(water_sensor_mesh_MeshPacket &p, water_sensor_mesh_NeighborInfo *n) override;

    /* Does our periodic broadcast */
    int32_t runOnce() override;

    /* Override wantPacket to say we want to see all packets when enabled, not just those for our port number.
      Exception is when the packet came via MQTT */
    virtual bool wantPacket(const water_sensor_mesh_MeshPacket *p) override { return enabled && !p->via_mqtt; }

    /* These are for debugging only */
    void printNeighborInfo(const char *header, const water_sensor_mesh_NeighborInfo *np);
    void printNodeDBNeighbors();
};
extern NeighborInfoModule *neighborInfoModule;