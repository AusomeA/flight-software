cmake_minimum_required(VERSION 3.20)

project(
    VirtualSatellite
    VERSION 0.1.0
    LANGUAGES CXX
)

find_package(
    Qt6
    REQUIRED COMPONENTS
    Quick
    Qml
    Network
)

qt_standard_project_setup(REQUIRES 6.5)

################### spacecraft simulator #######################
qt_add_executable(
    spacecraft_simulator
    apps/spacecraft_simulator/main.cpp
    apps/spacecraft_simulator/SpacecraftSimulator.h
    apps/spacecraft_simulator/SpacecraftSimulator.cpp
    libs/core/SharedTypes.h
    libs/ui/ReadoutsModel.h
    libs/ui/ReadoutsModel.cpp
    libs/ui/ReadoutFormatting.h
    libs/core/Helpers.h
    libs/protocol/TelemetryJson.h
    libs/networking/UdpReceiver.h
    libs/networking/UdpReceiver.cpp
)


qt_add_qml_module(
    spacecraft_simulator

    URI SpacecraftSimulator
    VERSION 0.1

    QML_FILES
    apps/spacecraft_simulator/ui/Main.qml
)

target_link_libraries(
    spacecraft_simulator
    PRIVATE
    Qt6::Quick
    Qt6::Qml
    Qt6::Network
)

target_compile_features(
    spacecraft_simulator
    PRIVATE
    cxx_std_20
)

target_include_directories(
    spacecraft_simulator
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/spacecraft_simulator
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/core
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/networking
)

################## flight computer ####################
qt_add_executable(
    flight_computer
    apps/flight_computer/main.cpp
    apps/flight_computer/FlightComputer.h
    apps/flight_computer/FlightComputer.cpp
    apps/flight_computer/FlightComputerUI.h
    apps/flight_computer/FlightComputerUI.cpp
    libs/core/SharedTypes.h
    libs/ui/ReadoutsModel.h
    libs/ui/ReadoutsModel.cpp
    libs/ui/ReadoutFormatting.h
    libs/core/Helpers.h
    libs/networking/UdpReceiver.h
    libs/networking/UdpReceiver.cpp
    libs/protocol/TelemetryJson.h
)

qt_add_qml_module(
    flight_computer

    URI FlightComputer
    VERSION 0.1

    QML_FILES
    apps/flight_computer/ui/Main.qml
)

target_link_libraries(
    flight_computer
    PRIVATE
    Qt6::Quick
    Qt6::Qml
    Qt6::Network
)

target_compile_features(
    flight_computer
    PRIVATE
    cxx_std_20
)

target_include_directories(
    flight_computer
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/flight_computer
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/core
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/networking
)




################# testing #####################
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
    vsat_tests
    tests/FlightComputerTests.cpp
    apps/flight_computer/FlightComputer.h
    apps/flight_computer/FlightComputer.cpp
    libs/core/SharedTypes.h
)

target_link_libraries(
    vsat_tests
    PRIVATE
    GTest::gtest_main
    Qt6::Qml
    Qt6::Network
)

target_include_directories(
    vsat_tests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/flight_computer
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/core
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/protocol
)

target_compile_features(
    vsat_tests
    PRIVATE
    cxx_std_20
)

include(GoogleTest)
gtest_discover_tests(vsat_tests)
