#pragma once

#include <mavlink-cpp/Mavlink.hpp>

void sendTunnelMessage(mavlink::Mavlink& mavlink, void* tunnelPayload, size_t tunnelPayloadSize);