#pragma once

#include "TelemetryJson.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <optional>

struct Envelope
{
    QString type;
    qint64 sequence = 0;
    QJsonObject body;
};

inline const QStringList requiredEnvelopeKeys = {"type", "sequence", "body"};

inline QByteArray EnvelopeToJson(const Envelope &envelope)
{
    QJsonObject json;
    json["type"] = envelope.type;
    json["sequence"] = envelope.sequence;
    json["body"] = envelope.body;

    return QJsonDocument(json).toJson(QJsonDocument::Compact); 
}

inline std::optional<Envelope> EnvelopeFromJson(const QByteArray &datagram)
{
    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(datagram, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
        return std::nullopt;

    QJsonObject json = document.object();

    if(!HasAllKeys(json, requiredEnvelopeKeys))
        return std::nullopt;

    if(!json["type"].isString() || json["type"].toString().isEmpty())
        return std::nullopt;

    if(!json["body"].isObject())
        return std::nullopt;

    const qint64 sequence = json["sequence"].toInteger(-1);
    if(sequence < 0)
        return std::nullopt;

    Envelope envelope;
    envelope.type = json["type"].toString();
    envelope.sequence = sequence;
    envelope.body = json["body"].toObject();

    return envelope;
}