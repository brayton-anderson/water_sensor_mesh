#pragma once
#include "ProtobufModule.h"

#define ROUTE_SIZE sizeof(((water_sensor_mesh_RouteDiscovery *)0)->route) / sizeof(((water_sensor_mesh_RouteDiscovery *)0)->route[0])

/**
 * A module that traces the route to a certain destination node
 */
class TraceRouteModule : public ProtobufModule<water_sensor_mesh_RouteDiscovery>
{
  public:
    TraceRouteModule();

  protected:
    bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_RouteDiscovery *r) override;

    virtual water_sensor_mesh_MeshPacket *allocReply() override;

    /* Called before rebroadcasting a RouteDiscovery payload in order to update
       the route array containing the IDs of nodes this packet went through */
    void alterReceivedProtobuf(water_sensor_mesh_MeshPacket &p, water_sensor_mesh_RouteDiscovery *r) override;

  private:
    // Call to add unknown hops (e.g. when a node couldn't decrypt it) to the route based on hopStart and current hopLimit
    void insertUnknownHops(water_sensor_mesh_MeshPacket &p, water_sensor_mesh_RouteDiscovery *r, bool isTowardsDestination);

    // Call to add your ID to the route array of a RouteDiscovery message
    void appendMyIDandSNR(water_sensor_mesh_RouteDiscovery *r, float snr, bool isTowardsDestination, bool SNRonly);

    /* Call to print the route array of a RouteDiscovery message.
       Set origin to where the request came from.
       Set dest to the ID of its destination, or NODENUM_BROADCAST if it has not yet arrived there. */
    void printRoute(water_sensor_mesh_RouteDiscovery *r, uint32_t origin, uint32_t dest, bool isTowardsDestination);
};

extern TraceRouteModule *traceRouteModule;