#include "FlightComputer.h"
#include <iostream>

using namespace std;

FlightComputer::FlightComputer()
: mode_(SharedTypes::Mode::nominal)
{}

SharedTypes::Commands FlightComputer::Update(const SharedTypes::Telemetry &telemetry)
{
    SharedTypes::Commands commands;
    commands.mode = mode_;

    cout << "Flight Computer Update" << endl;

    return commands;
}