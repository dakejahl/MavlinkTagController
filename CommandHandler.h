#pragma once

#include <Mavlink.hpp>

#include "TunnelProtocol.h"

class CommandHandler {
public:
    CommandHandler(mavlink::Mavlink& mavlink);

private:
    void _sendCommandAck        (uint32_t command, uint32_t result);
    void _handleTagCommand      (const mavlink_tunnel_t& tunnel);
    void _handleStartDetection  (void);
    void _handleStopDetection   (void);
    void _handleTunnelMessage   (const mavlink_message_t& message);


private:
    mavlink::Mavlink&           _mavlink;
    TunnelProtocol::TagInfo_t   _tagInfo;
};
