#pragma once
#include <vector>

#include "mesh/generated/water_sensor_mesh/admin.pb.h"
#include "mesh/generated/water_sensor_mesh/deviceonly.pb.h"
#include "mesh/generated/water_sensor_mesh/localonly.pb.h"
#include "mesh/generated/water_sensor_mesh/mesh.pb.h"

// this file defines constants which come from mesh.options

// Tricky macro to let you find the sizeof a type member
#define member_size(type, member) sizeof(((type *)0)->member)

/// max number of packets which can be waiting for delivery to android - note, this value comes from mesh.options protobuf
// FIXME - max_count is actually 32 but we save/load this as one long string of preencoded MeshPacket bytes - not a big array in
// RAM #define MAX_RX_TOPHONE (member_size(DeviceState, receive_queue) / member_size(DeviceState, receive_queue[0]))
#ifndef MAX_RX_TOPHONE
#define MAX_RX_TOPHONE 32
#endif

/// Verify baseline assumption of node size. If it increases, we need to reevaluate
/// the impact of its memory footprint, notably on MAX_NUM_NODES.
static_assert(sizeof(water_sensor_mesh_NodeInfoLite) <= 200, "NodeInfoLite size increased. Reconsider impact on MAX_NUM_NODES.");

/// max number of nodes allowed in the nodeDB
#ifndef MAX_NUM_NODES
#if defined(ARCH_STM32WL)
#define MAX_NUM_NODES 10
#elif defined(ARCH_NRF52)
#define MAX_NUM_NODES 80
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
#include "Esp.h"
static inline int get_max_num_nodes()
{
    uint32_t flash_size = ESP.getFlashChipSize() / (1024 * 1024); // Convert Bytes to MB
    if (flash_size >= 15) {
        return 250;
    } else if (flash_size >= 7) {
        return 200;
    } else {
        return 100;
    }
}
#define MAX_NUM_NODES get_max_num_nodes()
#else
#define MAX_NUM_NODES 100
#endif
#endif

/// Max number of channels allowed
#define MAX_NUM_CHANNELS (member_size(water_sensor_mesh_ChannelFile, channels) / member_size(water_sensor_mesh_ChannelFile, channels[0]))

/// helper function for encoding a record as a protobuf, any failures to encode are fatal and we will panic
/// returns the encoded packet size
size_t pb_encode_to_bytes(uint8_t *destbuf, size_t destbufsize, const pb_msgdesc_t *fields, const void *src_struct);

/// helper function for decoding a record as a protobuf, we will return false if the decoding failed
bool pb_decode_from_bytes(const uint8_t *srcbuf, size_t srcbufsize, const pb_msgdesc_t *fields, void *dest_struct);

/// Read from an Arduino File
bool readcb(pb_istream_t *stream, uint8_t *buf, size_t count);

/// Write to an arduino file
bool writecb(pb_ostream_t *stream, const uint8_t *buf, size_t count);

/** is_in_repeated is a macro/function that returns true if a specified word appears in a repeated protobuf array.
 * It relies on the following naming conventions from nanopb:
 *
 * pb_size_t ignore_incoming_count;
 * uint32_t ignore_incoming[3];
 */
bool is_in_helper(uint32_t n, const uint32_t *array, pb_size_t count);

#define is_in_repeated(name, n) is_in_helper(n, name, name##_count)