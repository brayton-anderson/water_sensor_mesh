#pragma once

#include "mesh/generated/water_sensor_mesh/mqtt.pb.h"

// water_sensor_mesh_ServiceEnvelope that automatically releases dynamically allocated memory when it goes out of scope.
struct DecodedServiceEnvelope : public water_sensor_mesh_ServiceEnvelope {
    DecodedServiceEnvelope(const uint8_t *payload, size_t length);
    DecodedServiceEnvelope(DecodedServiceEnvelope &) = delete;
    DecodedServiceEnvelope(DecodedServiceEnvelope &&);
    ~DecodedServiceEnvelope();
    // Clients must check that this is true before using.
    const bool validDecode;
};