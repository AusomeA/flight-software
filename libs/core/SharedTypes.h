#pragma once
#include <QObject>
#include <QQmlEngine>
#include <QUdpSocket>

namespace SharedTypes
{
    Q_NAMESPACE
    QML_ELEMENT

    inline constexpr quint16 telemetryPort = 45000;

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
        none
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
    };
}