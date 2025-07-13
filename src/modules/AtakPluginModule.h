#pragma once
#include "ProtobufModule.h"
#include "water_sensor_mesh/atak.pb.h"

/**
 * Waypoint message handling for water_sensor_mesh
 */
class AtakPluginModule : public ProtobufModule<water_sensor_mesh_TAKPacket>, private concurrency::OSThread
{
  public:
    /** Constructor
     * name is for debugging output
     */
    AtakPluginModule();

  protected:
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_TAKPacket *t) override;
    virtual void alterReceivedProtobuf(water_sensor_mesh_MeshPacket &mp, water_sensor_mesh_TAKPacket *t) override;
    /* Does our periodic broadcast */
    int32_t runOnce() override;

  private:
    water_sensor_mesh_TAKPacket cloneTAKPacketData(water_sensor_mesh_TAKPacket *t);
};

extern AtakPluginModule *atakPluginModule;