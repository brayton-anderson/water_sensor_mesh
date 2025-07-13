#include "ServiceEnvelope.h"
#include "mesh-pb-constants.h"
#include <pb_decode.h>

DecodedServiceEnvelope::DecodedServiceEnvelope(const uint8_t *payload, size_t length)
    : water_sensor_mesh_ServiceEnvelope(water_sensor_mesh_ServiceEnvelope_init_default),
      validDecode(pb_decode_from_bytes(payload, length, &water_sensor_mesh_ServiceEnvelope_msg, this))
{
}

DecodedServiceEnvelope::DecodedServiceEnvelope(DecodedServiceEnvelope &&other)
    : water_sensor_mesh_ServiceEnvelope(water_sensor_mesh_ServiceEnvelope_init_zero), validDecode(other.validDecode)
{
    std::swap(packet, other.packet);
    std::swap(channel_id, other.channel_id);
    std::swap(gateway_id, other.gateway_id);
}

DecodedServiceEnvelope::~DecodedServiceEnvelope()
{
    if (validDecode)
        pb_release(&water_sensor_mesh_ServiceEnvelope_msg, this);
}