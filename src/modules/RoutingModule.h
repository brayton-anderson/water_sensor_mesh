#pragma once
#include "Channels.h"
#include "ProtobufModule.h"

/**
 * Routing module for router control messages
 */
class RoutingModule : public ProtobufModule<water_sensor_mesh_Routing>
{
  public:
    /** Constructor
     * name is for debugging output
     */
    RoutingModule();

    virtual void sendAckNak(water_sensor_mesh_Routing_Error err, NodeNum to, PacketId idFrom, ChannelIndex chIndex,
                            uint8_t hopLimit = 0);

    // Given the hopStart and hopLimit upon reception of a request, return the hop limit to use for the response
    uint8_t getHopLimitForResponse(uint8_t hopStart, uint8_t hopLimit);

  protected:
    friend class Router;

    /** Called to handle a particular incoming message

    @return true if you've guaranteed you've handled this message and no other handlers should be considered for it
    */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_Routing *p) override;

    /** Messages can be received that have the want_response bit set.  If set, this callback will be invoked
     * so that subclasses can (optionally) send a response back to the original sender.  */
    virtual water_sensor_mesh_MeshPacket *allocReply() override;

    /// Override wantPacket to say we want to see all packets, not just those for our port number
    virtual bool wantPacket(const water_sensor_mesh_MeshPacket *p) override { return true; }
};

extern RoutingModule *routingModule;