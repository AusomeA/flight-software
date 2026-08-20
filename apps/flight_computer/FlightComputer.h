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
    SharedTypes::Telemetry telemetry_;

    double lastGroundContactSeconds_;

    static constexpr float maxSecondsWithoutGroundContact = 10800; // max amount of time without contact before we go into safe mode

    static constexpr float eclipseDurationSeconds = 2400.f;
    static constexpr float minSecondsUntilSunrise = eclipseDurationSeconds / 3.f; // no need to go to safe mode on low battery if sun is about to rise

    void ModeCheck();

    ///////// Testing Functions ///////////
public:
    void ModeTestUp();
    void ModeTestDown();
};