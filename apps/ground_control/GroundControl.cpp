#include "GroundControl.h"
#include "EnvelopeJson.h"
#include "TelemetryReadouts.h"

GroundControl::GroundControl(QObject *parent)
    : QObject(parent),
      telemetryReceiver_(SharedTypes::groundTelemetryPort)
{
    PopulateRows();
    connect(&telemetryReceiver_, &UdpReceiver::DatagramReceived, this, &GroundControl::HandleGroundTelemetry);

    timeSinceLastPacket_.start();
    connect(&linkCheckTimer_, &QTimer::timeout, this, &GroundControl::UpdateLinkRow);
    linkCheckTimer_.start(linkCheckIntervalMilliseconds);
}

void GroundControl::PopulateRows()
{
    readoutsModel_.SetRows(QVector<ReadoutRow>{
        {"Flight Computer Link", "", 0},
        {"Simulator Link", "", 0},
        {"Mode", "", 0},
    } + TelemetryReadouts());
}

void GroundControl::HandleGroundTelemetry(const QByteArray &payload)
{
    std::optional<Envelope> envelope = EnvelopeFromJson(payload);
    if(!envelope || envelope->type != SharedTypes::groundTelemetryMessageType)
    {
        qWarning() << "Dropped malformed ground telemetry packet";
        return;
    }

    std::optional<SharedTypes::Telemetry> telemetry = TelemetryFromJsonObject(envelope->body["telemetry"].toObject());
    const int modeNumber = envelope->body["mode"].toInteger(-1);

    if(!telemetry || modeNumber < 0 || modeNumber > static_cast<int>(SharedTypes::Mode::safe))
    {
        qWarning() << "Dropped malformed ground telemetry packet";
        return;
    }

    telemetry_ = *telemetry;
    mode_ = static_cast<SharedTypes::Mode>(modeNumber);
    simLinkOk_ = envelope->body["simLinkOk"].toBool();

    timeSinceLastPacket_.restart();
    UpdateRows();
}

void GroundControl::UpdateLinkRow()
{
    const qint64 silentMilliseconds = timeSinceLastPacket_.elapsed();
    const bool linkLost = silentMilliseconds > SharedTypes::linkLostMilliseconds;

    readoutsModel_.UpdateRow(flightComputerLinkRow, linkLost ? QString("No Link (%1 s)").arg(silentMilliseconds / 1000.0, 0, 'f', 1) 
                             : "Good Link", static_cast<int>(linkLost ? SharedTypes::Status::critical : SharedTypes::Status::good));

    if(linkLost)
        UpdateRows(true);
}

void GroundControl::UpdateRows(bool stale)
{
    auto rowStatus = [stale] (SharedTypes::Status status)
        { return static_cast<int>(stale ? SharedTypes::Status::stale : status);};

    readoutsModel_.UpdateRow(simulatorLinkRow, simLinkOk_ ? "Good Link" : "No Link", rowStatus(simLinkOk_ ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(modeRow, ModeText(mode_), rowStatus(GetModeStatus(mode_)));
    UpdateTelemetryReadouts(readoutsModel_, telemetry_, gcHeaderRowCount, stale);
}