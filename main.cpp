#include "CommandHandler.h"
#include "UDPPulseReceiver.h"
#include "startAirspyProcess.h"

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;

int main(int /*argc*/, char** /*argv[]*/)
{
    Mavsdk mavsdk;
    mavsdk.set_configuration(Mavsdk::Configuration(Mavsdk::Configuration::UsageType::CompanionComputer));

    ConnectionResult connection_result;
    connection_result = mavsdk.add_any_connection("udp://0.0.0.0:14561");
    if (connection_result != ConnectionResult::Success) {
        std::cout << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover Autopilot and QGC systems...\n";

    // Wait 3 seconds for the Autopilot System to show up
    auto autopilotPromise   = std::promise<std::shared_ptr<System>>{};
    auto autopilotFuture    = autopilotPromise.get_future();
    mavsdk.subscribe_on_new_system([&mavsdk, &autopilotPromise]() {
        auto system = mavsdk.systems().back();
        if (system->has_autopilot()) {
            std::cout << "Discovered Autopilot" << std::endl;
            autopilotPromise.set_value(system);
            mavsdk.subscribe_on_new_system(nullptr);            
        }
    });
    if (autopilotFuture.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        std::cerr << "No autopilot found, exiting.\n";
        return 1;
    }

    // Since we can't do anything without the QGC connection we wait for it indefinitely
    auto qgcPromise = std::promise<std::shared_ptr<System>>{};
    auto qgcFuture  = qgcPromise.get_future();
    mavsdk.subscribe_on_new_system([&mavsdk, &qgcPromise]() {
        auto system = mavsdk.systems().back();
        if (system->get_system_id() == 255) {
            std::cout << "Discovered QGC" << std::endl;
            qgcPromise.set_value(system);
            mavsdk.subscribe_on_new_system(nullptr);            
        }
    });
    qgcFuture.wait();

    // We have both systems ready for use now
    auto autopilotSystem    = autopilotFuture.get();
    auto qgcSystem          = qgcFuture.get();

    auto mavlinkPassthrough = MavlinkPassthrough{ qgcSystem };
    auto udpPulseReceiver   = UDPPulseReceiver{ "127.0.0.1", 30000, mavlinkPassthrough };
    
    CommandHandler{ *qgcSystem, mavlinkPassthrough };

    udpPulseReceiver.start();

    std::cout << "Ready" << std::endl;

    while (true) {
        udpPulseReceiver.receive();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

    return 0;
}
