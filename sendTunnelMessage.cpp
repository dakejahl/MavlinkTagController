#include "sendTunnelMessage.h"

void sendTunnelMessage(mavlink::Mavlink& mavlink, void* tunnelPayload, size_t tunnelPayloadSize)
{
    mavlink_message_t   message;
    mavlink_tunnel_t    tunnel;

    memset(&tunnel, 0, sizeof(tunnel));

    tunnel.target_system    = 255; // System id for QGC
    tunnel.target_component = MAV_COMP_ID_MISSIONPLANNER; // Comp id for QGC
    tunnel.payload_type     = MAV_TUNNEL_PAYLOAD_TYPE_UNKNOWN;
    tunnel.payload_length   = tunnelPayloadSize;

    memcpy(tunnel.payload, tunnelPayload, tunnelPayloadSize);

    mavlink_msg_tunnel_encode(
        mavlink.sysid(),
        mavlink.compid(),
        &message,
        &tunnel);
    mavlink.send_message(message);        	
}