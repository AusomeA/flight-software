#pragma once
#include <QString>
#include <QVariant>

enum class Status {
    good,
    warning,
    critical
};

class SpacecraftSimulator {
public:
    SpacecraftSimulator();

    QVariantList Readouts() const;
    QString StateText() const;
    void PrintState() const;

    void Start();
    void Stop(); 
    void Update(double deltaTimeSeconds);

    Status GetBatteryStatus() const;
    Status GetSolarGenerationStatus() const;
    Status GetPowerConsumptionStatus() const;
    Status GetTemperatureStatus() const;

private:

    bool isRunning_;

    double simulationTimeSeconds_;
    double updateIntervalSeconds_;

    float batteryPercentage_;
    float solarGenerationWatts_;
    float powerConsumptionWatts_;
    float temperatureCelsius_;

    bool isInSunlight_;
    bool communicationsAvailable_;

    bool temperatureSensorHealthy_;
    bool powerSensorHealthy_;
    bool attitudeSensorHealthy_;

    bool payloadEnabled_;

};