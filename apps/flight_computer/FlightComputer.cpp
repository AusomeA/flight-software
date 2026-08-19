#include "FlightComputer.h"
#include <iostream>

using namespace std;

FlightComputer::FlightComputer()
    : mode_(SharedTypes::Mode::nominal),
      lastGroundContactSeconds_(0.0)
{
}

SharedTypes::Commands FlightComputer::Update(const SharedTypes::Telemetry &telemetry)
{
    telemetry_ = telemetry;

    if(telemetry_.communicationsAvailable && telemetry_.commsTransmitting)
        lastGroundContactSeconds_ = telemetry_.missionElapsedTimeSeconds;

    ModeCheck();

    SharedTypes::Commands commands;
    commands.mode = mode_;
    return commands;
}

void FlightComputer::ModeCheck()
{
    if (mode_ == SharedTypes::Mode::safe)
        return;

    if (mode_ == SharedTypes::Mode::degraded)
    {
        if (telemetry_.batteryPercent > 45.f && telemetry_.temperatureSensorHealthy && telemetry_.powerSensorHealthy && telemetry_.attitudeSensorHealthy)
        {
            mode_ = SharedTypes::Mode::nominal;
            cout << "Spacecraft returned to nominal" << endl;
        }
    }

    QString safeModeReasons;

    if (telemetry_.batteryPercent < 25.f)
        safeModeReasons += "Battery below 25%\n";
    if (!telemetry_.temperatureSensorHealthy)
        safeModeReasons += "Temperature sensor failed\n";
    if (!telemetry_.powerSensorHealthy)
        safeModeReasons += "Power sensor failed\n";
    if (!telemetry_.attitudeSensorHealthy)
        safeModeReasons += "Attitude sensor failed\n";
    if (telemetry_.missionElapsedTimeSeconds - lastGroundContactSeconds_ > maxSecondsWithoutGroundContact)
        safeModeReasons += "No ground contact\n";

    if (!safeModeReasons.isEmpty())
    {
        mode_ = SharedTypes::Mode::safe;
        cout << "Spacecraft in safe mode because:" << endl
             << safeModeReasons.toStdString();
    }
    else if (telemetry_.batteryPercent < 35.f && !telemetry_.isInSunlight && telemetry_.secondsUntilSunrise > minSecondsUntilSunrise)
    {
        if (mode_ == SharedTypes::Mode::degraded)
            return;

        mode_ = SharedTypes::Mode::degraded;
        cout << "Spacecraft in degraded mode because:" << endl
             << "Battery below 35% and more than 1/3 of the eclipse remaining" << endl;
    }
}