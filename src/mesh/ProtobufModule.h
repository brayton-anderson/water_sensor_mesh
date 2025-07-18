#pragma once
#include "SinglePortModule.h"

/**
 * A base class for mesh modules that assume that they are sending/receiving one particular protobuf based
 * payload.  Using one particular app ID.
 *
 * If you are using protobufs to encode your packets (recommended) you can use this as a baseclass for your module
 * and avoid a bunch of boilerplate code.
 */
template <class T> class ProtobufModule : protected SinglePortModule
{
    const pb_msgdesc_t *fields;

  public:
    uint8_t numOnlineNodes = 0;
    /** Constructor
     * name is for debugging output
     */
    ProtobufModule(const char *_name, water_sensor_mesh_PortNum _ourPortNum, const pb_msgdesc_t *_fields)
        : SinglePortModule(_name, _ourPortNum), fields(_fields)
    {
    }

  protected:
    /**
     * Handle a received message, the data field in the message is already decoded and is provided
     *
     * In general decoded will always be !NULL.  But in some special applications (where you have handling packets
     * for multiple port numbers, decoding will ONLY be attempted for packets where the portnum matches our expected ourPortNum.
     */
    virtual bool handleReceivedProtobuf(const water_sensor_mesh_MeshPacket &mp, T *decoded) = 0;

    /** Called to make changes to a particular incoming message
     */
    virtual void alterReceivedProtobuf(water_sensor_mesh_MeshPacket &mp, T *decoded){};

    /**
     * Return a mesh packet which has been preinited with a particular protobuf data payload and port number.
     * You can then send this packet (after customizing any of the payload fields you might need) with
     * service->sendToMesh()
     */
    water_sensor_mesh_MeshPacket *allocDataProtobuf(const T &payload)
    {
        // Update our local node info with our position (even if we don't decide to update anyone else)
        water_sensor_mesh_MeshPacket *p = allocDataPacket();

        p->decoded.payload.size =
            pb_encode_to_bytes(p->decoded.payload.bytes, sizeof(p->decoded.payload.bytes), fields, &payload);
        // LOG_DEBUG("did encode");
        return p;
    }

    /**
     * Gets the short name from the sender of the mesh packet
     * Returns "???" if unknown sender
     */
    const char *getSenderShortName(const water_sensor_mesh_MeshPacket &mp)
    {
        auto node = nodeDB->getMeshNode(getFrom(&mp));
        const char *sender = (node) ? node->user.short_name : "???";
        return sender;
    }

    int handleStatusUpdate(const water_sensor_mesh::Status *arg)
    {
        if (arg->getStatusType() == STATUS_TYPE_NODE) {
            numOnlineNodes = nodeStatus->getNumOnline();
        }
        return 0;
    }

  private:
    /** Called to handle a particular incoming message

    @return ProcessMessage::STOP if you've guaranteed you've handled this message and no other handlers should be considered for
    it
    */
    virtual ProcessMessage handleReceived(const water_sensor_mesh_MeshPacket &mp) override
    {
        // FIXME - we currently update position data in the DB only if the message was a broadcast or destined to us
        // it would be better to update even if the message was destined to others.

        auto &p = mp.decoded;
        LOG_INFO("Received %s from=0x%0x, id=0x%x, portnum=%d, payloadlen=%d", name, mp.from, mp.id, p.portnum, p.payload.size);

        T scratch;
        T *decoded = NULL;
        if (mp.which_payload_variant == water_sensor_mesh_MeshPacket_decoded_tag && mp.decoded.portnum == ourPortNum) {
            memset(&scratch, 0, sizeof(scratch));
            if (pb_decode_from_bytes(p.payload.bytes, p.payload.size, fields, &scratch)) {
                decoded = &scratch;
            } else {
                LOG_ERROR("Error decoding proto module!");
                // if we can't decode it, nobody can process it!
                return ProcessMessage::STOP;
            }
        }

        return handleReceivedProtobuf(mp, decoded) ? ProcessMessage::STOP : ProcessMessage::CONTINUE;
    }

    /** Called to alter a particular incoming message
     */
    virtual void alterReceived(water_sensor_mesh_MeshPacket &mp) override
    {
        T scratch;
        T *decoded = NULL;
        if (mp.which_payload_variant == water_sensor_mesh_MeshPacket_decoded_tag && mp.decoded.portnum == ourPortNum) {
            memset(&scratch, 0, sizeof(scratch));
            const water_sensor_mesh_Data &p = mp.decoded;
            if (pb_decode_from_bytes(p.payload.bytes, p.payload.size, fields, &scratch)) {
                decoded = &scratch;
            } else {
                LOG_ERROR("Error decoding proto module!");
                // if we can't decode it, nobody can process it!
                return;
            }

            return alterReceivedProtobuf(mp, decoded);
        }
    }
};