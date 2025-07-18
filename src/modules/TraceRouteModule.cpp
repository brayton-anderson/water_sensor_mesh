#include "TraceRouteModule.h"
#include "MeshService.h"
#include "meshUtils.h"

TraceRouteModule *traceRouteModule;

bool TraceRouteModule::handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_RouteDiscovery *r)
{
    // We only alter the packet in alterReceivedProtobuf()
    return false; // let it be handled by RoutingModule
}

void TraceRouteModule::alterReceivedProtobuf(water_sensor_mesh_MeshPacket &p, water_sensor_mesh_RouteDiscovery *r)
{
    const water_sensor_mesh_Data &incoming = p.decoded;

    // Insert unknown hops if necessary
    insertUnknownHops(p, r, !incoming.request_id);

    // Append ID and SNR. If the last hop is to us, we only need to append the SNR
    appendMyIDandSNR(r, p.rx_snr, !incoming.request_id, isToUs(&p));
    if (!incoming.request_id)
        printRoute(r, p.from, p.to, true);
    else
        printRoute(r, p.to, p.from, false);

    // Set updated route to the payload of the to be flooded packet
    p.decoded.payload.size =
        pb_encode_to_bytes(p.decoded.payload.bytes, sizeof(p.decoded.payload.bytes), &water_sensor_mesh_RouteDiscovery_msg, r);
}

void TraceRouteModule::insertUnknownHops(water_sensor_mesh_MeshPacket &p, water_sensor_mesh_RouteDiscovery *r, bool isTowardsDestination)
{
    pb_size_t *route_count;
    uint32_t *route;
    pb_size_t *snr_count;
    int8_t *snr_list;

    // Pick the correct route array and SNR list
    if (isTowardsDestination) {
        route_count = &r->route_count;
        route = r->route;
        snr_count = &r->snr_towards_count;
        snr_list = r->snr_towards;
    } else {
        route_count = &r->route_back_count;
        route = r->route_back;
        snr_count = &r->snr_back_count;
        snr_list = r->snr_back;
    }

    // Only insert unknown hops if hop_start is valid
    if (p.hop_start != 0 && p.hop_limit <= p.hop_start) {
        uint8_t hopsTaken = p.hop_start - p.hop_limit;
        int8_t diff = hopsTaken - *route_count;
        for (uint8_t i = 0; i < diff; i++) {
            if (*route_count < ROUTE_SIZE) {
                route[*route_count] = NODENUM_BROADCAST; // This will represent an unknown hop
                *route_count += 1;
            }
        }
        // Add unknown SNR values if necessary
        diff = *route_count - *snr_count;
        for (uint8_t i = 0; i < diff; i++) {
            if (*snr_count < ROUTE_SIZE) {
                snr_list[*snr_count] = INT8_MIN; // This will represent an unknown SNR
                *snr_count += 1;
            }
        }
    }
}

void TraceRouteModule::appendMyIDandSNR(water_sensor_mesh_RouteDiscovery *updated, float snr, bool isTowardsDestination, bool SNRonly)
{
    pb_size_t *route_count;
    uint32_t *route;
    pb_size_t *snr_count;
    int8_t *snr_list;

    // Pick the correct route array and SNR list
    if (isTowardsDestination) {
        route_count = &updated->route_count;
        route = updated->route;
        snr_count = &updated->snr_towards_count;
        snr_list = updated->snr_towards;
    } else {
        route_count = &updated->route_back_count;
        route = updated->route_back;
        snr_count = &updated->snr_back_count;
        snr_list = updated->snr_back;
    }

    if (*snr_count < ROUTE_SIZE) {
        snr_list[*snr_count] = (int8_t)(snr * 4); // Convert SNR to 1 byte
        *snr_count += 1;
    }
    if (SNRonly)
        return;

    // Length of route array can normally not be exceeded due to the max. hop_limit of 7
    if (*route_count < ROUTE_SIZE) {
        route[*route_count] = myNodeInfo.my_node_num;
        *route_count += 1;
    } else {
        LOG_WARN("Route exceeded maximum hop limit!"); // Are you bridging networks?
    }
}

void TraceRouteModule::printRoute(water_sensor_mesh_RouteDiscovery *r, uint32_t origin, uint32_t dest, bool isTowardsDestination)
{
#ifdef DEBUG_PORT
    std::string route = "Route traced:\n";
    route += vformat("0x%x --> ", origin);
    for (uint8_t i = 0; i < r->route_count; i++) {
        if (i < r->snr_towards_count && r->snr_towards[i] != INT8_MIN)
            route += vformat("0x%x (%.2fdB) --> ", r->route[i], (float)r->snr_towards[i] / 4);
        else
            route += vformat("0x%x (?dB) --> ", r->route[i]);
    }
    // If we are the destination, or it has already reached the destination, print it
    if (dest == nodeDB->getNodeNum() || !isTowardsDestination) {
        if (r->snr_towards_count > 0 && r->snr_towards[r->snr_towards_count - 1] != INT8_MIN)
            route += vformat("0x%x (%.2fdB)", dest, (float)r->snr_towards[r->snr_towards_count - 1] / 4);

        else
            route += vformat("0x%x (?dB)", dest);
    } else
        route += "...";

    // If there's a route back (or we are the destination as then the route is complete), print it
    if (r->route_back_count > 0 || origin == nodeDB->getNodeNum()) {
        route += "\n";
        if (r->snr_towards_count > 0 && origin == nodeDB->getNodeNum())
            route += vformat("(%.2fdB) 0x%x <-- ", (float)r->snr_back[r->snr_back_count - 1] / 4, origin);
        else
            route += "...";

        for (int8_t i = r->route_back_count - 1; i >= 0; i--) {
            if (i < r->snr_back_count && r->snr_back[i] != INT8_MIN)
                route += vformat("(%.2fdB) 0x%x <-- ", (float)r->snr_back[i] / 4, r->route_back[i]);
            else
                route += vformat("(?dB) 0x%x <-- ", r->route_back[i]);
        }
        route += vformat("0x%x", dest);
    }
    LOG_INFO(route.c_str());
#endif
}

water_sensor_mesh_MeshPacket *TraceRouteModule::allocReply()
{
    assert(currentRequest);

    // Ignore multi-hop broadcast requests
    if (isBroadcast(currentRequest->to) && currentRequest->hop_limit < currentRequest->hop_start) {
        ignoreRequest = true;
        return NULL;
    }

    // Copy the payload of the current request
    auto req = *currentRequest;
    const auto &p = req.decoded;
    water_sensor_mesh_RouteDiscovery scratch;
    water_sensor_mesh_RouteDiscovery *updated = NULL;
    memset(&scratch, 0, sizeof(scratch));
    pb_decode_from_bytes(p.payload.bytes, p.payload.size, &water_sensor_mesh_RouteDiscovery_msg, &scratch);
    updated = &scratch;

    // Create a MeshPacket with this payload and set it as the reply
    water_sensor_mesh_MeshPacket *reply = allocDataProtobuf(*updated);

    return reply;
}

TraceRouteModule::TraceRouteModule()
    : ProtobufModule("traceroute", water_sensor_mesh_PortNum_TRACEROUTE_APP, &water_sensor_mesh_RouteDiscovery_msg)
{
    ourPortNum = water_sensor_mesh_PortNum_TRACEROUTE_APP;
    isPromiscuous = true; // We need to update the route even if it is not destined to us
}