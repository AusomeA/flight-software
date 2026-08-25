#pragma once
#include "SharedTypes.h"
#include <QJsonObject>
#include <QJsonDocument>

inline QByteArray TelemetryToJson(const SharedTypes::Telemetry &telemetry)
{
    QJsonObject json;
    json["missionElapsedTimeSeconds"] = telemetry.missionElapsedTimeSeconds;
    json["batteryPercent"]            = telemetry.batteryPercent;
    json["solarGenerationWatts"]      = telemetry.solarGenerationWatts;
    json["powerConsumptionWatts"]     = telemetry.powerConsumptionWatts;
    json["temperatureCelsius"]        = telemetry.temperatureCelsius;
    json["heaterEnabled"]             = telemetry.heaterEnabled;
    json["radiatorLouversOpen"]       = telemetry.radiatorLouversOpen;
    json["isInSunlight"]              = telemetry.isInSunlight;
    json["temperatureSensorHealthy"]  = telemetry.temperatureSensorHealthy;
    json["powerSensorHealthy"]        = telemetry.powerSensorHealthy;
    json["attitudeSensorHealthy"]     = telemetry.attitudeSensorHealthy;
    json["communicationsAvailable"]   = telemetry.communicationsAvailable;
    json["commsTransmitting"]         = telemetry.commsTransmitting;
    json["payloadEnabled"]            = telemetry.payloadEnabled;
    json["secondsUntilSunrise"]       = telemetry.secondsUntilSunrise;

    return QJsonDocument(json).toJson(QJsonDocument::Compact);      // Compact to remove whitespace
}

inline SharedTypes::Telemetry TelemetryFromJson(const QByteArray &payload)
{
    SharedTypes::Telemetry telemetry;
    QJsonObject json = QJsonDocument::fromJson(payload).object();

telemetry.missionElapsedTimeSeconds = json["missionElapsedTimeSeconds"].toDouble();
telemetry.batteryPercent            = json["batteryPercent"].toDouble();                // Json only has doubles
telemetry.solarGenerationWatts      = json["solarGenerationWatts"].toDouble();
telemetry.powerConsumptionWatts     = json["powerConsumptionWatts"].toDouble();
telemetry.temperatureCelsius        = json["temperatureCelsius"].toDouble();
telemetry.heaterEnabled             = json["heaterEnabled"].toBool();
telemetry.radiatorLouversOpen       = json["radiatorLouversOpen"].toBool();  
telemetry.isInSunlight              = json["isInSunlight"].toBool();
telemetry.temperatureSensorHealthy  = json["temperatureSensorHealthy"].toBool();
telemetry.powerSensorHealthy        = json["powerSensorHealthy"].toBool();
telemetry.attitudeSensorHealthy     = json["attitudeSensorHealthy"].toBool();
telemetry.communicationsAvailable   = json["communicationsAvailable"].toBool();
telemetry.commsTransmitting         = json["commsTransmitting"].toBool();
telemetry.payloadEnabled            = json["payloadEnabled"].toBool();
telemetry.secondsUntilSunrise       = json["secondsUntilSunrise"].toDouble();

return telemetry;
}

inline QByteArray CommandsToJson(const SharedTypes::Commands &commands)
{
    QJsonObject json;
    json["mode"] = static_cast<int>(commands.mode);
    return QJsonDocument(json).toJson(QJsonDocument::Compact);
}

inline SharedTypes::Commands CommandsFromJson(const QByteArray &payload)
{
    SharedTypes::Commands commands;
    QJsonObject json = QJsonDocument::fromJson(payload).object();
    commands.mode = static_cast<SharedTypes::Mode>(json["mode"].toInt());
    return commands;
}