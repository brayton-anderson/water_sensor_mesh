#include "mesh/generated/water_sensor_mesh/deviceonly.pb.h"
#include "mesh/generated/water_sensor_mesh/mesh.pb.h"

#pragma once
#include "NodeDB.h"

class TypeConversions
{
  public:
    static water_sensor_mesh_NodeInfo ConvertToNodeInfo(const water_sensor_mesh_NodeInfoLite *lite);
    static water_sensor_mesh_PositionLite ConvertToPositionLite(water_sensor_mesh_Position position);
    static water_sensor_mesh_Position ConvertToPosition(water_sensor_mesh_PositionLite lite);
    static water_sensor_mesh_UserLite ConvertToUserLite(water_sensor_mesh_User user);
    static water_sensor_mesh_User ConvertToUser(uint32_t nodeNum, water_sensor_mesh_UserLite lite);
};
