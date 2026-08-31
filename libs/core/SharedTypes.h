#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QUdpSocket>

namespace SharedTypes
{
    Q_NAMESPACE
    QML_ELEMENT

    inline constexpr quint16 telemetryPort  = 45000;
    inline constexpr quint16 commandPort    = 45001;
    inline constexpr quint16 discoveryPort  = 45010;

    static constexpr int discoveryIntervalMilliseconds = 1000;
    inline const QString simulatorName = "spacecraft_simulator";
    inline const QString flightComputerName = "flight_computer";
    inline const QString groundControlName = "ground_control";

    // Orbit times
    inline constexpr float orbitPeriodSeconds = 5400.f;
    inline constexpr float sunlitSeconds = 3000.f;
    inline constexpr float eclipseSeconds = orbitPeriodSeconds - sunlitSeconds;

    // payload windows
    inline constexpr float payloadStartTime = 900.f;
    inline constexpr float payloadEndTime = 1500.f;

    // Comms variables
    inline constexpr float commsStart = 1200.f;
    inline constexpr float commsEnd = 1800.f;
    inline constexpr float beaconPeriodSeconds = 150.f;
    inline constexpr float beaconTransmitSeconds = 15.f;

    inline constexpr int linkLostMilliseconds = 3000;

    // Heater variables
    inline constexpr float heaterOnCelsius = 15.f;
    inline constexpr float heaterOffCelsius = 25.f;

    // Battery thresholds
    inline constexpr float degradedEntryBatteryPercent = 50.f;
    inline constexpr float safeEntryBatteryPercent = 30.f;
    inline constexpr float nominalRecoveryBatteryPercent = 60.f;

    enum class Mode
    {
        nominal,
        degraded,
        safe
    };
    Q_ENUM_NS(Mode)

    enum class Status
    {
        good,
        warning,
        critical,
        none,
        stale
    };
    Q_ENUM_NS(Status)

    struct Telemetry
    {
        double missionElapsedTimeSeconds = 0.0;
        float batteryPercent = 0.f;
        float solarGenerationWatts = 0.f;
        float powerConsumptionWatts = 0.f;
        float temperatureCelsius = 0.f;
        bool heaterEnabled = false;
        bool radiatorLouversOpen = false;
        bool isInSunlight = false;
        bool temperatureSensorHealthy = true;
        bool powerSensorHealthy = true;
        bool attitudeSensorHealthy = true;
        bool communicationsAvailable = false;
        bool commsTransmitting = false;
        bool payloadEnabled = false;
        float secondsUntilSunrise = 0.f;
    };

    struct Commands
    {
        Mode mode = Mode::nominal;
        bool payloadEnabled = false;
        bool commsTransmitting = false;
        bool heaterEnabled = false;
    };
}