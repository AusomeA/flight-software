#include "SpacecraftSimulator.h"
#include <iostream>
#include <QTime>
#include <cmath>
#include <algorithm>

using namespace std;

SpacecraftSimulator::SpacecraftSimulator(QObject *parent)
    : QObject(parent),
      isRunning_(false),
      missionElapsedTimeSeconds_(0.0),
      updateIntervalSeconds_(1.0),
      timeScale_(1.f),
      batteryCapacityWattHours_(45.f),
      batteryEnergyWattHours_(batteryCapacityWattHours_),
      solarGenerationWatts_(40.f),
      powerConsumptionWatts_(20.f),
      temperatureCelsius_(22.f),
      isInSunlight_(false),
      communicationsAvailable_(true),
      temperatureSensorHealthy_(true),
      powerSensorHealthy_(true),
      attitudeSensorHealthy_(true),
      payloadEnabled_(false)
{
    connect(&updateTimer_, &QTimer::timeout, this, &SpacecraftSimulator::AdvanceOneTick);
}

QVariantList SpacecraftSimulator::Readouts() const
{
    QVariantList readouts;

    Status runningStatus = isRunning_ ? Status::good : Status::critical;

    QVariantList timeScaleRow{
        "Time Scale",
        QString("%1 x").arg(timeScale_, 0, 'f', 1),
        static_cast<int>(Status::none)};

    QVariantList runningRow{
        "Running",
        isRunning_ ? "Yes" : "No",
        static_cast<int>(runningStatus)};

    QVariantList METRow{
        "MET",
        MissionElapsedTimeText(),
        static_cast<int>(Status::none)};

    QVariantList updateIntervalRow{
        "Update Interval",
        QString("%1 s").arg(updateIntervalSeconds_, 0, 'f', 0),
        static_cast<int>(Status::none)};

    QVariantList batteryRow{
        "Battery",
        QString("%1 %").arg(BatteryCalculation(), 0, 'f', 1),
        static_cast<int>(GetBatteryStatus())};

    QVariantList solarGenerationRow{
        "Solar Generation",
        QString("%1 W").arg(solarGenerationWatts_, 0, 'f', 1),
        static_cast<int>(GetSolarGenerationStatus())};

    QVariantList powerConsumptionRow{
        "Power Consumption",
        QString("%1 W").arg(powerConsumptionWatts_, 0, 'f', 1),
        static_cast<int>(GetPowerConsumptionStatus())};

    QVariantList temperatureRow{
        "Temperature",
        QString("%1 C").arg(temperatureCelsius_, 0, 'f', 1),
        static_cast<int>(GetTemperatureStatus())};

    QVariantList sunlightRow{
        "In Sunlight",
        isInSunlight_ ? "Yes" : "No",
        static_cast<int>(isInSunlight_ ? Status::good : Status::none)};

    QVariantList commsRow{
        "Comms Available",
        communicationsAvailable_ ? "Yes" : "No",
        static_cast<int>(communicationsAvailable_ ? Status::good : Status::critical)};

    QVariantList temperatureSensorRow{
        "Temp Sensor OK",
        temperatureSensorHealthy_ ? "Yes" : "No",
        static_cast<int>(temperatureSensorHealthy_ ? Status::good : Status::critical)};

    QVariantList powerSensorRow{
        "Power Sensor OK",
        powerSensorHealthy_ ? "Yes" : "No",
        static_cast<int>(powerSensorHealthy_ ? Status::good : Status::critical)};

    QVariantList attitudeSensorRow{
        "Attitude Sensor OK",
        attitudeSensorHealthy_ ? "Yes" : "No",
        static_cast<int>(attitudeSensorHealthy_ ? Status::good : Status::critical)};

    QVariantList payloadRow{
        "Payload Enabled",
        payloadEnabled_ ? "Yes" : "No",
        static_cast<int>(payloadEnabled_ ? Status::good : Status::none)};

    readouts.append(QVariant(timeScaleRow));
    readouts.append(QVariant(runningRow));
    readouts.append(QVariant(METRow));
    readouts.append(QVariant(updateIntervalRow));
    readouts.append(QVariant(batteryRow));
    readouts.append(QVariant(solarGenerationRow));
    readouts.append(QVariant(powerConsumptionRow));
    readouts.append(QVariant(temperatureRow));
    readouts.append(QVariant(sunlightRow));
    readouts.append(QVariant(commsRow));
    readouts.append(QVariant(temperatureSensorRow));
    readouts.append(QVariant(powerSensorRow));
    readouts.append(QVariant(attitudeSensorRow));
    readouts.append(QVariant(payloadRow));

    return readouts;
}

QString SpacecraftSimulator::StateText() const
{
    QString text;

    if (isRunning_)
    {
        text += "Spacecraft State:\n";
        text += QString("  Running: %1\n").arg(isRunning_ ? "Yes" : "No");
        text += QString("  Mission Elapsed Time: %1 s\n").arg(missionElapsedTimeSeconds_);
        text += QString("  Update Interval: %1 s\n").arg(updateIntervalSeconds_);
        text += QString("  Battery: %1%\n").arg(BatteryCalculation());
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

    updateTimer_.start(static_cast<int>(updateIntervalSeconds_ * 1000));
    cout << "Simulation started." << endl;

    ///// Testing Variables //////
    // missionElapsedTimeSeconds_ = 0;
    // batteryPercentage_ = 20.f;
    // solarGenerationWatts_ = 10.01f;
    // powerConsumptionWatts_ = 34.9f;
    // temperatureCelsius_ = 29.9f;
    // isInSunlight_ = false;
    // communicationsAvailable_ = false;
    // temperatureSensorHealthy_ = false;
    // powerSensorHealthy_ = false;
    // attitudeSensorHealthy_ = false;
    // payloadEnabled_ = false;

    emit readoutsChanged();
}

void SpacecraftSimulator::Stop()
{
    isRunning_ = false;
    updateTimer_.stop();
    cout << "Simulation stopped." << endl;
    emit readoutsChanged();
}

void SpacecraftSimulator::Update(double deltaTimeSeconds)
{
    if (!isRunning_)
    {
        cout << "Simulation is not running. Cannot update." << endl;
        return;
    }

    missionElapsedTimeSeconds_ += deltaTimeSeconds;

    // batteryPercentage_ = 20.f;
    // solarGenerationWatts_ = 10.01f;
    // powerConsumptionWatts_ = 34.9f;

    // temperatureCelsius_ = 29.9f;
    // isInSunlight_ = false;

    // communicationsAvailable_ = false;
    // temperatureSensorHealthy_ = false;
    // powerSensorHealthy_ = false;
    // attitudeSensorHealthy_ = false;
    // payloadEnabled_ = false;

    // Sun Calculation
    float timeIntoOrbit = fmod(missionElapsedTimeSeconds_, orbitPeriodSeconds);
    isInSunlight_ = timeIntoOrbit >= eclipsePeriod;

    // Watts Calculations
    if (isInSunlight_)
        solarGenerationWatts_ = 40.f;
    else
        solarGenerationWatts_ = 0.f;

    ///////// power consumtion Calculations go here ////////////
    // powerConsumptionWatts_ =;

    // Battery Calculations
    float netPowerWatts = solarGenerationWatts_ - powerConsumptionWatts_;

    batteryEnergyWattHours_ += netPowerWatts * (deltaTimeSeconds / 3600);
    batteryEnergyWattHours_ = qBound(0.f, batteryEnergyWattHours_, batteryCapacityWattHours_);

    // temperature calculation
    float targetTemperature = (isInSunlight_ ? sunlitEquilibriumCelsius : eclipseEquilibriumCelsius) + powerConsumptionWatts_ * celsiusPerWatt;
    temperatureCelsius_ += (targetTemperature - temperatureCelsius_) * (deltaTimeSeconds / thermalTimeConstantSeconds);
}

void SpacecraftSimulator::IncreaseTimeScale()
{
    if (timeScale_ != maxTimeScale)
    {
        timeScale_ = min(timeScale_ * 2.0, static_cast<double>(maxTimeScale));
        cout << "Time Scale = " << timeScale_ << endl;
        emit timeScaleChanged();
    }
}

void SpacecraftSimulator::DecreaseTimeScale()
{
    if (timeScale_ != minTimeScale)
    {
        timeScale_ = max(timeScale_ / 2.0, static_cast<double>(minTimeScale));
        cout << "Time Scale = " << timeScale_ << endl;
        emit timeScaleChanged();
    }
}

SpacecraftSimulator::Status SpacecraftSimulator::GetBatteryStatus() const
{
    float batteryPercentage = BatteryCalculation();

    if (batteryPercentage > 50.f)
        return Status::good;
    else if (batteryPercentage > 20.f)
        return Status::warning;
    else
        return Status::critical;
}

SpacecraftSimulator::Status SpacecraftSimulator::GetSolarGenerationStatus() const
{
    if (!isInSunlight_)
        return Status::none;

    if (solarGenerationWatts_ > 30.f)
        return Status::good;
    else if (solarGenerationWatts_ > 10.f)
        return Status::warning;
    else
        return Status::critical;
}

SpacecraftSimulator::Status SpacecraftSimulator::GetPowerConsumptionStatus() const
{
    if (powerConsumptionWatts_ < 25.f)
        return Status::good;
    else if (powerConsumptionWatts_ < 35.f)
        return Status::warning;
    else
        return Status::critical;
}

SpacecraftSimulator::Status SpacecraftSimulator::GetTemperatureStatus() const
{
    if (temperatureCelsius_ <= maxTemperatureGoodCelsius && temperatureCelsius_ >= minTemperatureGoodCelsius)
        return Status::good;
    else if (temperatureCelsius_ <= maxTemperatureWarningCelsius && temperatureCelsius_ >= minTemperatureWarningCelsius)
        return Status::warning;
    else
        return Status::critical;
}

QString SpacecraftSimulator::MissionElapsedTimeText() const
{
    QString METText = QString("T+ ");

    int totalSeconds = static_cast<int>(missionElapsedTimeSeconds_); // will wrap around at ~ 68 years (~2 billion seconds)

    int days = totalSeconds / 86400;
    int hours = (totalSeconds % 86400) / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    return METText + QString("%1:%2:%3:%4")
                         .arg(days, 3, 10, QChar('0'))
                         .arg(hours, 2, 10, QChar('0'))
                         .arg(minutes, 2, 10, QChar('0'))
                         .arg(seconds, 2, 10, QChar('0'));
}

void SpacecraftSimulator::AdvanceOneTick()
{
    Update(updateIntervalSeconds_ * timeScale_);
    emit readoutsChanged();
}