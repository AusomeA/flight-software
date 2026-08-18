#pragma once
#include "SharedTypes.h"

class FlightComputer
{

public:
    FlightComputer();

    SharedTypes::Commands Update(const SharedTypes::Telemetry &telemetry);

    SharedTypes::Mode GetMode() const { return mode_; }

private:
    SharedTypes::Mode mode_;
};