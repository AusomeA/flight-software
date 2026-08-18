#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QString>
#include "SharedTypes.h"
#include "ReadoutsModel.h"

enum ReadoutRowIndex
{
    timeScaleRow,
    modeRow,
    runningRow,
    METRow,
    batteryRow,
    solarGenerationRow,
    powerConsumptionRow,
    temperatureRow,
    heaterRow,
    radiatorRow,
    sunlightRow,
    commsRow,
    commsTransmittingRow,
    temperatureSensorRow,
    powerSensorRow,
    attitudeSensorRow,
    payloadRow,
    readoutRowCount
};

class SpacecraftSimulator : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Spacecraft)
    Q_PROPERTY(double timeScale READ TimeScale NOTIFY timeScaleChanged)
    Q_PROPERTY(QAbstractItemModel *readoutsModel READ ReadoutsModelPtr CONSTANT)

public:
    enum class Status
    {
        good,
        warning,
        critical,
        none
    };
    Q_ENUM(Status)

    SpacecraftSimulator(QObject *parent = nullptr);

    QString StateText() const;
    void PrintState() const;

    Q_INVOKABLE void Start();
    Q_INVOKABLE void Stop();
    void Update(double deltaTimeSeconds);

    Q_INVOKABLE void IncreaseTimeScale();
    Q_INVOKABLE void DecreaseTimeScale();
    double TimeScale() const { return timeScale_; }

    Q_INVOKABLE void ToggleSensorFault(int sensorIndex);
    Q_INVOKABLE void FailRandomSensor(int randomNumber);

    Q_INVOKABLE void ToggleChaosMode();

    Status GetBatteryStatus() const;
    Status GetSolarGenerationStatus() const;
    Status GetPowerConsumptionStatus() const;
    Status GetTemperatureStatus() const;

    SharedTypes::Mode GetMode() const { return mode_; }
    void SetMode(SharedTypes::Mode inMode) { mode_ = inMode; }

    QAbstractItemModel *ReadoutsModelPtr() { return &readoutsModel_; }

    /////// Testing Functions ////////////
    Q_INVOKABLE void ModeTestUp();
    Q_INVOKABLE void ModeTestDown();
    Q_INVOKABLE void BatteryTestUp();
    Q_INVOKABLE void BatteryTestDown();
    /////////////////////////////////////

signals:
    void timeScaleChanged();

private:
    // Variables
    SharedTypes::Mode mode_;

    bool isRunning_;

    double missionElapsedTimeSeconds_;
    float updateIntervalSeconds_;
    float timeScale_;

    float batteryCapacityWattHours_; // Total capacity of battery
    float batteryEnergyWattHours_;   // How much juice is in the battery

    float solarGenerationWatts_;
    float powerConsumptionWatts_;
    float temperatureCelsius_;

    bool isInSunlight_;

    bool communicationsAvailable_;
    bool temperatureSensorHealthy_;
    bool powerSensorHealthy_;
    bool attitudeSensorHealthy_;

    bool payloadEnabled_;
    bool commsTransmitting_;
    bool heaterEnabled_;
    bool radiatorLouversOpen_;

    bool chaosEnabled_;
    static constexpr float meanSecondsBetweenFaults = 300.f; // the mean seconds before a fault happens in chaos mode

    ReadoutsModel readoutsModel_;

    // Sunlight Variables
    static constexpr float orbitPeriodSeconds = 5400.f;                        // 5400 seconds = 90 minutes
    static constexpr float sunlitSeconds = 3000.f;                             // 3000 seconds = 50 minutes
    static constexpr float eclipsePeriod = orbitPeriodSeconds - sunlitSeconds; // 90 - 50 = 40 minutes (subject to change if other values change)

    // Power Consumption Variables
    static constexpr float basePowerConsumption = 5.f; // how much power is consumed, even in safe mode
    static constexpr float avionicsPowerConsumption = 8.f;
    static constexpr float payloadPowerConsumption = 12.f;
    static constexpr float commsPowerConsumption = 10.f;
    static constexpr float heaterPowerConsumption = 10.f;

    // Temperature Variables
    static constexpr float sunlitEquilibriumCelsius = 32.5f;    // what temp it settles in sunlight
    static constexpr float eclipseEquilibriumCelsius = -5.f;    // what temp it settles in darkness
    static constexpr float thermalTimeConstantSeconds = 600.f;  // how fast it responds
    static constexpr float celsiusPerWatt = 0.8f;               // internal heat from electronics (including heater)
    static constexpr float louverCoolingCelsius = 10.f;         // how much the radiator cools
    static constexpr float minTemperatureGoodCelsius = 10.f;    // what temp is still good (minimum)
    static constexpr float maxTemperatureGoodCelsius = 30.f;    // what temp is still good (maximum)
    static constexpr float minTemperatureWarningCelsius = 5.f;  // what temp is still just a warning (minimum)
    static constexpr float maxTemperatureWarningCelsius = 36.f; // what temp is still just a warning (maximum)

    // Time Scale Variables
    static constexpr float minTimeScale = .5f;
    static constexpr float maxTimeScale = 64.f;
    // Payload Variables
    static constexpr float payloadStartTime = 900.f; // payload begins working start and end windows
    static constexpr float payloadEndTime = 1500.f;

    // Comms Variables
    static constexpr float commsStart = 1200.f; // comms available start and end windows
    static constexpr float commsEnd = 1800.f;

    static constexpr float beaconPeriodSeconds = 150.f;  // safe mode beacon times
    static constexpr float beaconTransmitSeconds = 15.f; // transmit for this many seconds every beacon period

    // Functions
    QString MissionElapsedTimeText() const;

    QTimer updateTimer_;

    float BatteryCalculation() const { return batteryEnergyWattHours_ / batteryCapacityWattHours_ * 100.f; }

    float TimeIntoOrbit() const { return fmod(missionElapsedTimeSeconds_, orbitPeriodSeconds); }

    void AdvanceOneTick();

    void PayloadCheck();
    void CommsCheck();
    void HeaterCheck();
    void RadiatorCheck();
    void ModeCheck();

    void PopulateReadouts();
    void UpdateReadouts();
};