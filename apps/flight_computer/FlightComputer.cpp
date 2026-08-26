#include "FlightComputer.h"
#include <iostream>

using namespace std;

FlightComputer::FlightComputer()
    : mode_(SharedTypes::Mode::nominal),
      payloadEnabled_(false),
      commsTransmitting_(false),
      lastGroundContactSeconds_(0.0)
{
}

SharedTypes::Commands FlightComputer::Update(const SharedTypes::Telemetry &telemetry)
{
    telemetry_ = telemetry;

    if (telemetry_.communicationsAvailable && telemetry_.commsTransmitting)
        lastGroundContactSeconds_ = telemetry_.missionElapsedTimeSeconds;

    ModeCheck();
    PayloadCheck();
    CommsCheck();

    SharedTypes::Commands commands;
    commands.mode = mode_;
    commands.payloadEnabled = payloadEnabled_;
    commands.commsTransmitting = commsTransmitting_;
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

float FlightComputer::TimeIntoOrbit() const
{
    return fmod(telemetry_.missionElapsedTimeSeconds, SharedTypes::orbitPeriodSeconds);
}

void FlightComputer::PayloadCheck()
{
    if (mode_ != SharedTypes::Mode::nominal)
    {
        if (payloadEnabled_)
            payloadEnabled_ = false;

        return;
    }

    if (!payloadEnabled_)
    {
        float timeInOrbit = TimeIntoOrbit();
        if (timeInOrbit >= SharedTypes::payloadStartTime && timeInOrbit <= SharedTypes::payloadEndTime && telemetry_.batteryPercent > minPayloadBatteryPercent)
        {
            payloadEnabled_ = true;
            cout << "Payload Enabled" << endl;
        }
    }
    else
    {
        float timeInOrbit = TimeIntoOrbit();
        if (timeInOrbit > SharedTypes::payloadEndTime || timeInOrbit < SharedTypes::payloadStartTime)
        {
            payloadEnabled_ = false;
            cout << "Payload Disabled" << endl;
        }
    }
}

void FlightComputer::CommsCheck()
{
    float timeInOrbit = TimeIntoOrbit();

    if (mode_ == SharedTypes::Mode::safe)
    {
        if (!commsTransmitting_)
        {
            float timeIntoBeaconCycle = fmod(telemetry_.missionElapsedTimeSeconds, SharedTypes::beaconPeriodSeconds);
            if (timeIntoBeaconCycle <= SharedTypes::beaconTransmitSeconds)
            {
                commsTransmitting_ = true;
                cout << "Comms Transmitting in Safe Mode (Beacon Mode)" << endl;
            }
        }
        else
        {
            float timeIntoBeaconCycle = fmod(telemetry_.missionElapsedTimeSeconds, SharedTypes::beaconPeriodSeconds);
            if (timeIntoBeaconCycle > SharedTypes::beaconTransmitSeconds)
            {
                commsTransmitting_ = false;
                cout << "Comms Stopped Transmitting in Safe Mode (Beacon Mode)" << endl;
            }
        }
        return;
    }

    if (mode_ == SharedTypes::Mode::degraded)
    {
        float commsQuarterWindow = (SharedTypes::commsEnd - SharedTypes::commsStart) / 4;

        if (!commsTransmitting_)
        {
            if (timeInOrbit >= SharedTypes::commsStart + commsQuarterWindow && timeInOrbit <= SharedTypes::commsEnd - commsQuarterWindow)
            {
                commsTransmitting_ = true;
                cout << "Comms Transmitting in Degraded Mode" << endl;
            }
        }
        else
        {
            if (timeInOrbit < SharedTypes::commsStart + commsQuarterWindow || timeInOrbit > SharedTypes::commsEnd - commsQuarterWindow)
            {
                commsTransmitting_ = false;
                cout << "Comms Stopped Transmitting in Degraded Mode" << endl;
            }
        }
        return;
    }

    if (!commsTransmitting_)
    {
        if (timeInOrbit >= SharedTypes::commsStart && timeInOrbit <= SharedTypes::commsEnd && telemetry_.batteryPercent > minCommsBatteryPercent)
        {
            commsTransmitting_ = true;
            cout << "Comms Transmitting..." << endl;
        }
    }
    else
    {
        if (timeInOrbit > SharedTypes::commsEnd || timeInOrbit < SharedTypes::commsStart)
        {
            commsTransmitting_ = false;
            cout << "Comms Stopped Transmitting" << endl;
        }
    }
}




/////////////////////////////////// Testing Functions /////////////////////////////////////////////////
void FlightComputer::ModeTestUp()
{
    if (mode_ == SharedTypes::Mode::nominal)
    {
        mode_ = SharedTypes::Mode::degraded;
        cout << "mode changed to degraded" << endl;
        lastGroundContactSeconds_ = telemetry_.missionElapsedTimeSeconds;
    }
    else if (mode_ == SharedTypes::Mode::degraded)
    {
        mode_ = SharedTypes::Mode::safe;
        cout << "mode changed to safe" << endl;
        lastGroundContactSeconds_ = telemetry_.missionElapsedTimeSeconds;
    }
}

void FlightComputer::ModeTestDown()
{
    if (mode_ == SharedTypes::Mode::safe)
    {
        mode_ = SharedTypes::Mode::degraded;
        cout << "mode changed to degraded" << endl;
        lastGroundContactSeconds_ = telemetry_.missionElapsedTimeSeconds;
    }
    else if (mode_ == SharedTypes::Mode::degraded)
    {
        mode_ = SharedTypes::Mode::nominal;
        cout << "mode changed to nominal" << endl;
        lastGroundContactSeconds_ = telemetry_.missionElapsedTimeSeconds;
    }
}