#pragma once

// low level types

#include "MemoryPool.h"
#include "mesh/mesh-pb-constants.h"
#include <Arduino.h>

typedef uint32_t NodeNum;
typedef uint32_t PacketId; // A packet sequence number

#define NODENUM_BROADCAST UINT32_MAX
#define NODENUM_BROADCAST_NO_LORA                                                                                                \
    1 // Reserved to only deliver packets over high speed (non-lora) transports, such as MQTT or BLE mesh (not yet implemented)
#define ERRNO_OK 0
#define ERRNO_NO_INTERFACES 33
#define ERRNO_UNKNOWN 32                   // pick something that doesn't conflict with RH_ROUTER_ERROR_UNABLE_TO_DELIVER
#define ERRNO_DISABLED 34                  // the interface is disabled
#define ERRNO_SHOULD_RELEASE 35            // no error, but the packet should still be released
#define ID_COUNTER_MASK (UINT32_MAX >> 22) // mask to select the counter portion of the ID

/*
 * Source of a received message
 */
enum RxSource {
    RX_SRC_LOCAL, // message was generated locally
    RX_SRC_RADIO, // message was received from radio mesh
    RX_SRC_USER   // message was received from end-user device
};

/**
 * the max number of hops a message can pass through, used as the default max for hop_limit in MeshPacket.
 *
 * We reserve 3 bits in the header so this could be up to 7, but given the high range of lora and typical usecases, keeping
 * maxhops to 3 should be fine for a while.  This also serves to prevent routing/flooding attempts to be attempted for
 * too long.
 **/
#define HOP_MAX 7

/// We normally just use max 3 hops for sending reliable messages
#define HOP_RELIABLE 3

// For old firmware or when falling back to flooding, there is no next-hop preference
#define NO_NEXT_HOP_PREFERENCE 0
// For old firmware there is no relay node set
#define NO_RELAY_NODE 0

typedef int ErrorCode;

/// Alloc and free packets to our global, ISR safe pool
extern Allocator<water_sensor_mesh_MeshPacket> &packetPool;
using UniquePacketPoolPacket = Allocator<water_sensor_mesh_MeshPacket>::UniqueAllocation;

/**
 * Most (but not always) of the time we want to treat packets 'from' the local phone (where from == 0), as if they originated on
 * the local node. If from is zero this function returns our node number instead
 */
NodeNum getFrom(const water_sensor_mesh_MeshPacket *p);

// Returns true if the packet originated from the local node
bool isFromUs(const water_sensor_mesh_MeshPacket *p);

// Returns true if the packet is destined to us
bool isToUs(const water_sensor_mesh_MeshPacket *p);

/* Some clients might not properly set priority, therefore we fix it here. */
void fixPriority(water_sensor_mesh_MeshPacket *p);

bool isBroadcast(uint32_t dest);