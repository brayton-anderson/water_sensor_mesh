#pragma once

#include <Arduino.h>

#include "mesh/generated/water_sensor_mesh/mesh.pb.h" // For CriticalErrorCode

/// A macro that include filename and line
#define RECORD_CRITICALERROR(code) recordCriticalError(code, __LINE__, __FILE__)

/// Record an error that should be reported via analytics
void recordCriticalError(water_sensor_mesh_CriticalErrorCode code = water_sensor_mesh_CriticalErrorCode_UNSPECIFIED, uint32_t address = 0,
                         const char *filename = NULL);