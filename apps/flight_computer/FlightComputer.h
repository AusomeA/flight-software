#pragma once
#include "SharedTypes.h"

class FlightComputer
{

public:
    FlightComputer();

    SharedTypes::Commands Update(const SharedTypes::Telemetry &telemetry);

    SharedTypes::Mode GetMode() const { return mode_; }

    const SharedTypes::Telemetry &GetTelemetry() const { return telemetry_; }

private:
    SharedTypes::Mode mode_;
    bool payloadEnabled_;
    bool commsTransmitting_;
    bool heaterEnabled_; // FC's own last-commanded state - deliberately NOT read from telemetry

    SharedTypes::Telemetry telemetry_;

    double lastGroundContactSeconds_;

    static constexpr float maxSecondsWithoutGroundContact = 10800; // max amount of time without contact before we go into safe mode

    static constexpr float minSecondsUntilSunrise = SharedTypes::eclipseSeconds / 3.f; // no need to go to safe mode on low battery if sun is about to rise

    static constexpr float minCommsBatteryPercent = 10.f;

    void ModeCheck();
    void PayloadCheck();
    void CommsCheck();
    void HeaterCheck();

    float TimeIntoOrbit() const;

    ///////// Testing Functions ///////////
public:
    void ModeTestUp();
    void ModeTestDown();
};