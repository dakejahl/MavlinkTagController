#include "CommandHandler.h"
#include "UDPPulseReceiver.h"
#include "startAirspyProcess.h"

#include <Mavlink.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavlink;

int main(int /*argc*/, char** /*argv[]*/)
{
    ConfigurationSettings udpConfigSettings = {
        .connection_url = "udp://127.0.0.1:14540",
        .sysid          = 1,
        .compid         = MAV_COMP_ID_ONBOARD_COMPUTER,
        .mav_type       = MAV_TYPE_GENERIC,
        .mav_autopilot  = MAV_AUTOPILOT_INVALID,
        .emit_heartbeat = true
    };
    ConfigurationSettings serialConfigSettings = {
        .connection_url = "serial:///dev/ttyS0:921600",
        .sysid          = 1,
        .compid         = MAV_COMP_ID_ONBOARD_COMPUTER,
        .mav_type       = MAV_TYPE_GENERIC,
        .mav_autopilot  = MAV_AUTOPILOT_INVALID,
        .emit_heartbeat = true
    };

    ConnectionResult connection_result;
    auto mavlink = std::make_shared<Mavlink>(serialConfigSettings);
    connection_result = mavlink->start();
    if (connection_result == ConnectionResult::Success) {
        std::cout << "Connected to ttyS0" << std::endl;
    } else {
        std::cout << "Connection failed for ttyS0: " << connection_result << std::endl;
        std::cout << "Connecting to udp instead" << std::endl;
        mavlink = std::make_shared<Mavlink>(udpConfigSettings);
        connection_result = mavlink->start();
        if (connection_result == ConnectionResult::Success) {
        std::cout << "Connected to udp" << std::endl;
        } else {
            std::cout << "Connection failed for udp: " << connection_result << std::endl;
            return 1;
        }
    }

    std::cout << "Waiting to discover Autopilot...\n";
    while (!mavlink->connected()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto udpPulseReceiver = UDPPulseReceiver{ "127.0.0.1", 30000, *mavlink };
    
    CommandHandler{ *mavlink };

    udpPulseReceiver.start();

    std::cout << "Ready" << std::endl;

    while (true) {
        udpPulseReceiver.receive();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
