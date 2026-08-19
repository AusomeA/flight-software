#include "SpacecraftSimulator.h"
#include <iostream>
#include <QTime>
#include <cmath>
#include <algorithm>
#include <QRandomGenerator>

using namespace std;

SpacecraftSimulator::SpacecraftSimulator(QObject *parent)
    : QObject(parent),
      mode_(SharedTypes::Mode::nominal),
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
      payloadEnabled_(false),
      commsTransmitting_(false),
      heaterEnabled_(false),
      radiatorLouversOpen_(false),
      chaosEnabled_(false)
{
    connect(&updateTimer_, &QTimer::timeout, this, &SpacecraftSimulator::AdvanceOneTick);
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

    PopulateReadouts();
}

void SpacecraftSimulator::Stop()
{
    isRunning_ = false;
    updateTimer_.stop();
    cout << "Simulation stopped." << endl;
    UpdateReadouts();
}

void SpacecraftSimulator::Update(double deltaTimeSeconds)
{
    if (!isRunning_)
    {
        cout << "Simulation is not running. Cannot update." << endl;
        return;
    }

    missionElapsedTimeSeconds_ += deltaTimeSeconds;

    // sun calculation
    isInSunlight_ = TimeIntoOrbit() > eclipsePeriod;

    // fault injection
    if (chaosEnabled_ && QRandomGenerator::global()->generateDouble() < deltaTimeSeconds / meanSecondsBetweenFaults)
        FailRandomSensor(QRandomGenerator::global()->bounded(3));

    //////////////////// flight computer (for testing, delete later)//////////////////////
    SharedTypes::Commands commands = flightComputer_.Update(BuildTelemetry());
    mode_ = commands.mode;

    // power checks
    PayloadCheck();
    CommsCheck();
    HeaterCheck();
    RadiatorCheck();

    // watts calculations
    if (isInSunlight_)
        solarGenerationWatts_ = 40.f;
    else
        solarGenerationWatts_ = 0.f;

    // power consumtion calculations
    powerConsumptionWatts_ = basePowerConsumption + avionicsPowerConsumption;
    if (payloadEnabled_)
        powerConsumptionWatts_ += payloadPowerConsumption;
    if (commsTransmitting_)
        powerConsumptionWatts_ += commsPowerConsumption;
    if (heaterEnabled_)
        powerConsumptionWatts_ += heaterPowerConsumption;

    // battery calculations
    float netPowerWatts = solarGenerationWatts_ - powerConsumptionWatts_;

    batteryEnergyWattHours_ += netPowerWatts * (deltaTimeSeconds / 3600);
    batteryEnergyWattHours_ = qBound(0.f, batteryEnergyWattHours_, batteryCapacityWattHours_);

    // temperature calculation
    float targetTemperature = (isInSunlight_ ? sunlitEquilibriumCelsius : eclipseEquilibriumCelsius) + powerConsumptionWatts_ * celsiusPerWatt; // where the temperature settles
    if (radiatorLouversOpen_)
        targetTemperature -= louverCoolingCelsius;
    float thermalResponse = 1.f - std::exp(-deltaTimeSeconds / thermalTimeConstantSeconds); // needed to calculate exactly (even with increased time scale)
    temperatureCelsius_ += (targetTemperature - temperatureCelsius_) * thermalResponse;
}

void SpacecraftSimulator::IncreaseTimeScale()
{
    if (timeScale_ != maxTimeScale)
    {
        timeScale_ = min(timeScale_ * 2.0, static_cast<double>(maxTimeScale));
        cout << "Time Scale = " << timeScale_ << endl;
        emit timeScaleChanged();
        UpdateReadouts();
    }
}

void SpacecraftSimulator::DecreaseTimeScale()
{
    if (timeScale_ != minTimeScale)
    {
        timeScale_ = max(timeScale_ / 2.0, static_cast<double>(minTimeScale));
        cout << "Time Scale = " << timeScale_ << endl;
        emit timeScaleChanged();
        UpdateReadouts();
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
    if (powerConsumptionWatts_ < 33.75f) // if ran through entire eclipse, would break even on power consumption
        return Status::good;
    else if (powerConsumptionWatts_ < 46.f) // 45 watts is everything running at once. Should not go above 45 watts and should only use 45 watts sparingly.
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
    float simulatedSeconds = updateIntervalSeconds_ * timeScale_;

    while (simulatedSeconds > 0.0)
    {
        float physicsStepSeconds = min(simulatedSeconds, 1.f);
        Update(physicsStepSeconds);
        simulatedSeconds -= physicsStepSeconds;
    }

    UpdateReadouts();
}

SharedTypes::Telemetry SpacecraftSimulator::BuildTelemetry() const
{
    SharedTypes::Telemetry telemetry;
    telemetry.missionElapsedTimeSeconds = missionElapsedTimeSeconds_;
    telemetry.batteryPercent = BatteryCalculation();
    telemetry.temperatureCelsius = temperatureCelsius_;
    telemetry.isInSunlight = isInSunlight_;
    telemetry.secondsUntilSunrise = isInSunlight_ ? 0.f : eclipsePeriod - TimeIntoOrbit();
    telemetry.temperatureSensorHealthy = temperatureSensorHealthy_;
    telemetry.powerSensorHealthy = powerSensorHealthy_;
    telemetry.attitudeSensorHealthy = attitudeSensorHealthy_;
    telemetry.communicationsAvailable = communicationsAvailable_;
    telemetry.commsTransmitting = commsTransmitting_;
    return telemetry;
}

void SpacecraftSimulator::PayloadCheck()
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
        if (timeInOrbit >= payloadStartTime && timeInOrbit <= payloadEndTime && BatteryCalculation() > 30.f)
        {
            payloadEnabled_ = true;
            cout << "Payload Enabled" << endl;
        }
    }
    else
    {
        float timeInOrbit = TimeIntoOrbit();
        if (timeInOrbit > payloadEndTime || timeInOrbit < payloadStartTime)
        {
            payloadEnabled_ = false;
            cout << "Payload Disabled" << endl;
        }
    }
}

void SpacecraftSimulator::CommsCheck()
{
    float timeInOrbit = TimeIntoOrbit();
    communicationsAvailable_ = timeInOrbit >= commsStart && timeInOrbit <= commsEnd;

    if (mode_ == SharedTypes::Mode::safe)
    {
        if (!commsTransmitting_)
        {
            float timeIntoBeaconCycle = fmod(missionElapsedTimeSeconds_, beaconPeriodSeconds);
            if (timeIntoBeaconCycle <= beaconTransmitSeconds)
            {
                commsTransmitting_ = true;
                cout << "Comms Transmitting in Safe Mode (Beacon Mode)" << endl;
            }
        }
        else
        {
            float timeIntoBeaconCycle = fmod(missionElapsedTimeSeconds_, beaconPeriodSeconds);
            if (timeIntoBeaconCycle > beaconTransmitSeconds)
            {
                commsTransmitting_ = false;
                cout << "Comms Stopped Transmitting in Safe Mode (Beacon Mode)" << endl;
            }
        }
        return;
    }

    if (mode_ == SharedTypes::Mode::degraded)
    {
        float commsQuarterWindow = (commsEnd - commsStart) / 4;

        if (!commsTransmitting_)
        {
            if (timeInOrbit >= commsStart + commsQuarterWindow && timeInOrbit <= commsEnd - commsQuarterWindow)
            {
                commsTransmitting_ = true;
                cout << "Comms Transmitting in Degraded Mode" << endl;
            }
        }
        else
        {
            if (timeInOrbit < commsStart + commsQuarterWindow || timeInOrbit > commsEnd - commsQuarterWindow)
            {
                commsTransmitting_ = false;
                cout << "Comms Stopped Transmitting in Degraded Mode" << endl;
            }
        }
        return;
    }

    if (!commsTransmitting_)
    {
        if (timeInOrbit >= commsStart && timeInOrbit <= commsEnd && BatteryCalculation() > 10.f)
        {
            commsTransmitting_ = true;
            cout << "Comms Transmitting..." << endl;
        }
    }
    else
    {
        if (timeInOrbit > commsEnd || timeInOrbit < commsStart)
        {
            commsTransmitting_ = false;
            cout << "Comms Stopped Transmitting" << endl;
        }
    }
}

void SpacecraftSimulator::HeaterCheck()
{
    if (!heaterEnabled_ && !isInSunlight_ && temperatureCelsius_ <= 15.f)
    {
        heaterEnabled_ = true;
        cout << "Heater turned on" << endl;
    }
    else if (heaterEnabled_ && (isInSunlight_ || temperatureCelsius_ > 25.f))
    {
        heaterEnabled_ = false;
        cout << "Heater turned off" << endl;
    }
}

void SpacecraftSimulator::RadiatorCheck()
{
    if (!radiatorLouversOpen_ && isInSunlight_ && temperatureCelsius_ >= 25.f)
    {
        radiatorLouversOpen_ = true;
        cout << "Radiator louvers opened" << endl;
    }
    else if (radiatorLouversOpen_ && (!isInSunlight_ || temperatureCelsius_ < 20.f))
    {
        radiatorLouversOpen_ = false;
        cout << "Radiator louvers closed" << endl;
    }
}

void SpacecraftSimulator::PopulateReadouts()
{
    QVector<ReadoutRow> rows{
        {"Time Scale", "", 0},
        {"Mode", "", 0},
        {"Running", "", 0},
        {"MET", "", 0},
        {"Battery", "", 0},
        {"Solar Generation", "", 0},
        {"Power Consumption", "", 0},
        {"Temperature", "", 0},
        {"Heater", "", 0},
        {"Radiator Louvers", "", 0},
        {"In Sunlight", "", 0},
        {"Comms Available", "", 0},
        {"Comms Transmitting", "", 0},
        {"Temp Sensor OK", "", 0},
        {"Power Sensor OK", "", 0},
        {"Attitude Sensor OK", "", 0},
        {"Payload Enabled", "", 0},
        {"Chaos Enabled", "", 0}};

    readoutsModel_.SetRows(rows);
    UpdateReadouts();
}

void SpacecraftSimulator::UpdateReadouts()
{
    Status runningStatus = isRunning_ ? Status::good : Status::critical;

    readoutsModel_.UpdateRow(timeScaleRow, QString("%1 x").arg(timeScale_, 0, 'f', 1), static_cast<int>(Status::none));
    readoutsModel_.UpdateRow(modeRow, mode_ == SharedTypes::Mode::nominal ? "Nominal" : mode_ == SharedTypes::Mode::degraded ? "Degraded"
                                                                                                                             : "Safe",
                             static_cast<int>(mode_));
    readoutsModel_.UpdateRow(runningRow, isRunning_ ? "Yes" : "No", static_cast<int>(runningStatus));
    readoutsModel_.UpdateRow(METRow, MissionElapsedTimeText(), static_cast<int>(Status::none));
    readoutsModel_.UpdateRow(batteryRow, QString("%1 %").arg(BatteryCalculation(), 0, 'f', 1), static_cast<int>(GetBatteryStatus()));
    readoutsModel_.UpdateRow(solarGenerationRow, QString("%1 W").arg(solarGenerationWatts_, 0, 'f', 1), static_cast<int>(GetSolarGenerationStatus()));
    readoutsModel_.UpdateRow(powerConsumptionRow, QString("%1 W").arg(powerConsumptionWatts_, 0, 'f', 1), static_cast<int>(GetPowerConsumptionStatus()));
    readoutsModel_.UpdateRow(temperatureRow, QString("%1 C").arg(temperatureCelsius_, 0, 'f', 1), static_cast<int>(GetTemperatureStatus()));
    readoutsModel_.UpdateRow(heaterRow, heaterEnabled_ ? "On" : "Off", static_cast<int>(Status::none));
    readoutsModel_.UpdateRow(radiatorRow, radiatorLouversOpen_ ? "Open" : "Shut", static_cast<int>(Status::none));
    readoutsModel_.UpdateRow(sunlightRow, isInSunlight_ ? "Yes" : "No", static_cast<int>(isInSunlight_ ? Status::good : Status::none));
    readoutsModel_.UpdateRow(commsRow, communicationsAvailable_ ? "Yes" : "No", static_cast<int>(communicationsAvailable_ ? Status::good : Status::none));
    readoutsModel_.UpdateRow(commsTransmittingRow, commsTransmitting_ ? "Yes" : "No", static_cast<int>(commsTransmitting_ ? Status::good : Status::none));
    readoutsModel_.UpdateRow(temperatureSensorRow, temperatureSensorHealthy_ ? "Yes" : "No", static_cast<int>(temperatureSensorHealthy_ ? Status::good : Status::critical));
    readoutsModel_.UpdateRow(powerSensorRow, powerSensorHealthy_ ? "Yes" : "No", static_cast<int>(powerSensorHealthy_ ? Status::good : Status::critical));
    readoutsModel_.UpdateRow(attitudeSensorRow, attitudeSensorHealthy_ ? "Yes" : "No", static_cast<int>(attitudeSensorHealthy_ ? Status::good : Status::critical));
    readoutsModel_.UpdateRow(payloadRow, payloadEnabled_ ? "Yes" : "No", static_cast<int>(payloadEnabled_ ? Status::good : Status::none));
    readoutsModel_.UpdateRow(chaosRow, chaosEnabled_ ? "Yes" : "No", static_cast<int>(Status::none));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Testing Functions ////////////////////////////////////////////////////

void SpacecraftSimulator::ModeTestUp()
{
    flightComputer_.ModeTestUp();
    mode_ = flightComputer_.GetMode();
    UpdateReadouts();
}

void SpacecraftSimulator::ModeTestDown()
{
    flightComputer_.ModeTestDown();
    mode_ = flightComputer_.GetMode();
    UpdateReadouts();
}

void SpacecraftSimulator::BatteryTestUp()
{
    batteryEnergyWattHours_ += 10.f;
    cout << "Battery added by 10 for testing" << endl;
    UpdateReadouts();
}

void SpacecraftSimulator::BatteryTestDown()
{
    batteryEnergyWattHours_ -= 10.f;
    cout << "Battery dropped by 10 for testing" << endl;
    UpdateReadouts();
}

void SpacecraftSimulator::ToggleSensorFault(int sensorIndex)
{
    switch (sensorIndex)
    {
    case 0:
        temperatureSensorHealthy_ = !temperatureSensorHealthy_;
        cout << "temperature sensor healthy is " << (temperatureSensorHealthy_ ? "true" : "false") << endl;
        break;
    case 1:
        powerSensorHealthy_ = !powerSensorHealthy_;
        cout << "power sensor healthy is " << (powerSensorHealthy_ ? "true" : "false") << endl;
        break;
    case 2:
        attitudeSensorHealthy_ = !attitudeSensorHealthy_;
        cout << "attitude sensor healthy is " << (attitudeSensorHealthy_ ? "true" : "false") << endl;
        break;
    default:
        cout << "incorrect input" << endl;
        break;
    }
}

void SpacecraftSimulator::FailRandomSensor(int randomNumber)
{
    switch (randomNumber)
    {
    case 0:
        temperatureSensorHealthy_ = false;
        cout << "temperature sensor failed" << endl;
        break;
    case 1:
        powerSensorHealthy_ = false;
        cout << "power sensor failed" << endl;
        break;
    case 2:
        attitudeSensorHealthy_ = false;
        cout << "attitude sensor failed" << endl;
        break;
    default:
        cout << "random failure function failed (wrong index?)" << endl;
        break;
    }
}

void SpacecraftSimulator::ToggleChaosMode()
{
    chaosEnabled_ = !chaosEnabled_;
    cout << "Chaos mode " << (chaosEnabled_ ? "on" : "off") << endl;
}