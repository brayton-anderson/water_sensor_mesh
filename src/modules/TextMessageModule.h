#pragma once
#include "Observer.h"
#include "SinglePortModule.h"

/**
 * Text message handling for water_sensor_mesh - draws on the OLED display the most recent received message
 */
class TextMessageModule : public SinglePortModule, public Observable<const water_sensor_mesh_MeshPacket *>
{
  public:
    /** Constructor
     * name is for debugging output
     */
    TextMessageModule() : SinglePortModule("text", water_sensor_mesh_PortNum_TEXT_MESSAGE_APP) {}

  protected:
    /** Called to handle a particular incoming message

    @return ProcessMessage::STOP if you've guaranteed you've handled this message and no other handlers should be considered for
    it
    */
    virtual ProcessMessage handleReceived(const water_sensor_mesh_MeshPacket &mp) override;
    virtual bool wantPacket(const water_sensor_mesh_MeshPacket *p) override;
};

extern TextMessageModule *textMessageModule;