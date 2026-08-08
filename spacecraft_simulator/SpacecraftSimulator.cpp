#include "SpacecraftSimulator.h"
#include <iostream>

using namespace std;

SpacecraftSimulator::SpacecraftSimulator()
    : isRunning_(false),
      simulationTimeSeconds_(0.0),
      updateIntervalSeconds_(1.0),
      batteryPercentage_(100.0f),
      solarGenerationWatts_(40.0f),
      powerConsumptionWatts_(20.0f),
      temperatureCelsius_(22.0f),
      isInSunlight_(true),
      communicationsAvailable_(true),
      temperatureSensorHealthy_(true),
      powerSensorHealthy_(true),
      attitudeSensorHealthy_(true),
      payloadEnabled_(false)
{}

QVariantList SpacecraftSimulator::Readouts() const
{
    QVariantList readouts;

    Status runningStatus = isRunning_ ? Status::good : Status::critical;

    QVariantList runningRow{
        "Running",
        isRunning_ ? "Yes" : "No",
        static_cast<int>(runningStatus)
    };

    readouts.append(QVariant(runningRow));

    return readouts;
}

QString SpacecraftSimulator::StateText() const
{
    QString text;

    if (isRunning_)
    {
        text += "Spacecraft State:\n";
        text += QString("  Running: %1\n").arg(isRunning_ ? "Yes" : "No");
        text += QString("  Simulation Time: %1 s\n").arg(simulationTimeSeconds_);
        text += QString("  Update Interval: %1 s\n").arg(updateIntervalSeconds_);
        text += QString("  Battery: %1%\n").arg(batteryPercentage_);
        text += QString("  Solar Generation: %1 W\n").arg(solarGenerationWatts_);
        text += QString("  Power Consumption: %1 W\n").arg(powerConsumptionWatts_);
        text += QString("  Temperature: %1 C\n").arg(temperatureCelsius_);
        text += QString("  In Sunlight: %1\n").arg(isInSunlight_ ? "Yes" : "No");
        text += QString("  Comms Available: %1\n").arg(communicationsAvailable_ ? "Yes" : "No");
        text += QString("  Temp Sensor OK: %1\n").arg(temperatureSensorHealthy_ ? "Yes" : "No");
        text += QString("  Power Sensor OK: %1\n").arg(powerSensorHealthy_ ? "Yes" : "No");
        text += QString("  Attitude Sensor OK: %1\n").arg(attitudeSensorHealthy_ ? "Yes" : "No");
        text += QString("  Payload Enabled: %1\n").arg(payloadEnabled_ ? "Yes" : "No");
    }
    else
        text = "Simulation is not running. No state to display.\n";

    return text;
}

void SpacecraftSimulator::PrintState() const
{
    cout << StateText().toStdString() << endl;
}

void SpacecraftSimulator::Start()
{
    isRunning_ = true;
    simulationTimeSeconds_ = 0.0;
    cout << "Simulation started." << endl;
}

void SpacecraftSimulator::Stop()
{
    isRunning_ = false;
}

void SpacecraftSimulator::Update(double deltaTimeSeconds)
{
    if (!isRunning_)
    {
        cout << "Simulation is not running. Cannot update." << endl;
        return;
    }

    simulationTimeSeconds_ += deltaTimeSeconds;
}

Status SpacecraftSimulator::GetBatteryStatus() const
{
    if (batteryPercentage_ > 50.0f)
        return Status::good;
    else if (batteryPercentage_ > 20.0f)
        return Status::warning;
    else
        return Status::critical;
}

Status SpacecraftSimulator::GetSolarGenerationStatus() const
{
    if (solarGenerationWatts_ > 30.0f)
        return Status::good;
    else if (solarGenerationWatts_ > 10.0f)
        return Status::warning;
    else
        return Status::critical;
}

Status SpacecraftSimulator::GetPowerConsumptionStatus() const
{
    if (powerConsumptionWatts_ < 25.0f)
        return Status::good;
    else if (powerConsumptionWatts_ < 35.0f)
        return Status::warning;
    else
        return Status::critical;
}

Status SpacecraftSimulator::GetTemperatureStatus() const
{
    if (temperatureCelsius_ < 25.0f)
        return Status::good;
    else if (temperatureCelsius_ < 30.0f)
        return Status::warning;
    else
        return Status::critical;
}
