#pragma once

#include <Mavlink.hpp>

void sendTunnelMessage(mavlink::Mavlink& mavlink, void* tunnelPayload, size_t tunnelPayloadSize);