#include "sendTunnelMessage.h"

void sendTunnelMessage(MavlinkPassthrough& mavlinkPassthrough, void* tunnelPayload, size_t tunnelPayloadSize)
{
    mavlink_message_t   message;
    mavlink_tunnel_t    tunnel;

    memset(&tunnel, 0, sizeof(tunnel));

    tunnel.target_system    = _mavlinkPassthrough.get_target_sysid();
    tunnel.target_component = _mavlinkPassthrough.get_target_compid();
    tunnel.payload_type     = MAV_TUNNEL_PAYLOAD_TYPE_UNKNOWN;
    tunnel.payload_length   = tunnelPayloadSize;

    memcpy(tunnel.payload, tunnelPayload, tunnelPayloadSize);

    mavlink_msg_tunnel_encode(
        _mavlinkPassthrough.get_our_sysid(),
        _mavlinkPassthrough.get_our_compid(),
        &message,
        &tunnel);
    _mavlinkPassthrough.send_message(message);        	
}