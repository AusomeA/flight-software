#pragma once
#include <QObject>
#include <QQmlEngine>

namespace SharedTypes
{
    Q_NAMESPACE
    QML_ELEMENT

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
        float temperatureCelsius = 0.f;
        bool isInSunlight = false;
        bool temperatureSensorHealthy = true;
        bool powerSensorHealthy = true;
        bool attitudeSensorHealthy = true;
        bool communicationsAvailable = false;
        bool commsTransmitting = false;
        float secondsUntilSunrise = 0.f;
    };

    struct Commands
    {
        Mode mode = Mode::nominal;
    };
}