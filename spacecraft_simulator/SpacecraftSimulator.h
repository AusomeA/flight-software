#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QString>
#include <QVariant>

enum class Status
{
    good,
    warning,
    critical,
    none
};

class SpacecraftSimulator : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Spacecraft)
    Q_PROPERTY(QVariantList readouts READ Readouts NOTIFY readoutsChanged)

public:
    SpacecraftSimulator(QObject *parent = nullptr);

    QVariantList Readouts() const;
    QString StateText() const;
    void PrintState() const;

    Q_INVOKABLE void Start();
    Q_INVOKABLE void Stop();
    void Update(double deltaTimeSeconds);

    Status GetBatteryStatus() const;
    Status GetSolarGenerationStatus() const;
    Status GetPowerConsumptionStatus() const;
    Status GetTemperatureStatus() const;

signals:
    void readoutsChanged();

private:
// Variables
bool isRunning_;

double missionElapsedTimeSeconds_;
double updateIntervalSeconds_;

float batteryCapacityWattHours_;
float batteryEnergyWattHours_;

float solarGenerationWatts_;
float powerConsumptionWatts_;
float temperatureCelsius_;

bool isInSunlight_;

bool communicationsAvailable_;
bool temperatureSensorHealthy_;
bool powerSensorHealthy_;
bool attitudeSensorHealthy_;

bool payloadEnabled_;


static constexpr float orbitPeriodSeconds = 5400.f; // 90 minutes
static constexpr float sunlitSeconds = 3000.f;      // 50 minutes
static constexpr float eclipsePeriod = orbitPeriodSeconds - sunlitSeconds; // 90 - 50 = 40 minutes

static constexpr float sunlitEquilibriumCelsius = 35.f;    // where it settles in sunlight
static constexpr float eclipseEquilibriumCelsius = -5.f;   // where it settles in darkness
static constexpr float thermalTimeConstantSeconds = 600.f; // how fast it responds
static constexpr float celsiusPerWatt = 0.3f;              // internal heat from electronics

static constexpr float temperatureGoodLowCelsius = 10.f;        
static constexpr float temperatureGoodHighCelsius = 30.f;
static constexpr float temperatureWarningLowCelsius = 5.f;
static constexpr float temperatureWarningHighCelsius = 35.f;

// Functions
QString MissionElapsedTimeText() const;

QTimer updateTimer_;

float BatteryCalculation() const {return batteryEnergyWattHours_ / batteryCapacityWattHours_ * 100.f;}

};