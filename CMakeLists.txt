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
)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(
    spacecraft_simulator
    spacecraft_simulator/main.cpp
    spacecraft_simulator/SpacecraftSimulator.cpp
    spacecraft_simulator/SpacecraftSimulator.h
)

qt_add_qml_module(
    spacecraft_simulator

    URI SpacecraftSimulator
    VERSION 1.0

    QML_FILES
        spacecraft_simulator/ui/Main.qml
)

target_link_libraries(
    spacecraft_simulator
    PRIVATE
        Qt6::Quick
        Qt6::Qml
)

target_compile_features(
    spacecraft_simulator
    PRIVATE
        cxx_std_17
)

target_include_directories(
    spacecraft_simulator
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/spacecraft_simulator
)