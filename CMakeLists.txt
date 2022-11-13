cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member")

project(MavlinkTagController)

add_definitions("-Wall -Wextra")

add_subdirectory(libraries)

add_executable(MavlinkTagController
    main.cpp
    CommandHandler.cpp CommandHandler.h
    UDPPulseReceiver.cpp UDPPulseReceiver.h
    startAirspyProcess.cpp startAirspyProcess.h
    sendTunnelMessage.cpp sendTunnelMessage.h
    TunnelProtocol.h
)

target_link_libraries(MavlinkTagController
    mavlink-cpp
    airspyhf
)
