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

    faultsModel_.SetRows({{"Temperature Sensor Fault", "Off", static_cast<int>(SharedTypes::Status::none)},
                          {"Power Sensor Fault", "Off", static_cast<int>(SharedTypes::Status::none)},
                          {"Attitude Sensor Fault", "Off", static_cast<int>(SharedTypes::Status::none)}});
    connect(&godSender_, &AckUdpSender::Acknowledged, this, &GroundControl::HandleFaultAck);
    connect(&godSender_, &AckUdpSender::GaveUp, this, &GroundControl::HandleFaultGaveUp);
}

void GroundControl::PopulateRows()
{
    readoutsModel_.SetRows(QVector<ReadoutRow>{
                               {"Flight Computer Link", "", 0},
                               {"Simulator Link", "", 0},
                               {"Mode", "", 0},
                           } +
                           TelemetryReadouts());
}

void GroundControl::HandleGroundTelemetry(const QByteArray &payload)
{
    std::optional<Envelope> envelope = EnvelopeFromJson(payload);
    if (!envelope || envelope->type != SharedTypes::groundTelemetryMessageType)
    {
        qWarning() << "Dropped malformed ground telemetry packet";
        return;
    }

    std::optional<SharedTypes::Telemetry> telemetry = TelemetryFromJsonObject(envelope->body["telemetry"].toObject());
    const int modeNumber = envelope->body["mode"].toInteger(-1);

    if (!telemetry || modeNumber < 0 || modeNumber > static_cast<int>(SharedTypes::Mode::safe))
    {
        qWarning() << "Dropped malformed ground telemetry packet";
        return;
    }

    telemetry_ = *telemetry;
    mode_ = static_cast<SharedTypes::Mode>(modeNumber);
    simLinkOk_ = envelope->body["simLinkOk"].toBool();

    timeSinceLastPacket_.restart();
    UpdateRows();
    emit summaryChanged();
}

void GroundControl::UpdateLinkRow()
{
    const qint64 silentMilliseconds = timeSinceLastPacket_.elapsed();
    const bool linkLost = silentMilliseconds > SharedTypes::linkLostMilliseconds;
    flightComputerLinked_ = !linkLost;

    readoutsModel_.UpdateRow(flightComputerLinkRow, linkLost ? QString("No Link (%1 s)").arg(silentMilliseconds / 1000.0, 0, 'f', 1) : "Good Link", static_cast<int>(linkLost ? SharedTypes::Status::critical : SharedTypes::Status::good));

    if (linkLost)
        UpdateRows(true);

    emit summaryChanged();
}

void GroundControl::UpdateRows(bool stale)
{
    auto rowStatus = [stale](SharedTypes::Status status)
    { return static_cast<int>(stale ? SharedTypes::Status::stale : status); };

    readoutsModel_.UpdateRow(simulatorLinkRow, simLinkOk_ ? "Good Link" : "No Link", rowStatus(simLinkOk_ ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(modeRow, ModeText(mode_), rowStatus(GetModeStatus(mode_)));
    UpdateTelemetryReadouts(readoutsModel_, telemetry_, gcHeaderRowCount, stale);
}

QString GroundControl::FaultName(int faultRow)
{
    switch (faultRow)
    {
    case temperatureSensorFaultRow:
        return SharedTypes::temperatureSensorFaultMessage;
    case powerSensorFaultRow:
        return SharedTypes::powerSensorFaultMessage;
    case attitudeSensorFaultRow:
        return SharedTypes::attitudeSensorFaultMessage;
    default:
        return QString();
    }
}

void GroundControl::SetFault(int faultRow, bool active)
{
    const QString faultName = FaultName(faultRow);
    if (faultName.isEmpty())
    {
        qWarning() << "Unknown fault row" << faultRow;
        return;
    }

    const QList<QHostAddress> simulators = discovery_.LivePeerAddresses(SharedTypes::simulatorName);

    if (simulators.isEmpty())
    {
        faultsModel_.UpdateRow(faultRow, "No Simulator", static_cast<int>(SharedTypes::Status::critical));
        return;
    }

    QJsonObject body;
    body["fault"] = faultName;
    body["active"] = active;

    const qint64 sequence = godSender_.SendAck(SharedTypes::faultInjectionMessageType, body, simulators.first(), SharedTypes::godPort);
    pendingFaults_[sequence] = {faultRow, active};
    faultsModel_.UpdateRow(faultRow, active ? "Turning On..." : "Turning Off...", static_cast<int>(SharedTypes::Status::warning));
}

void GroundControl::HandleFaultAck(qint64 sequence, bool accepted)
{
    if (!pendingFaults_.contains(sequence))
        return;

    const PendingFault fault = pendingFaults_.take(sequence);

    if (accepted)
        faultsModel_.UpdateRow(fault.row, fault.active ? "On" : "Off", static_cast<int>(fault.active ? SharedTypes::Status::critical : SharedTypes::Status::none));
    else
        faultsModel_.UpdateRow(fault.row, "Rejected", static_cast<int>(SharedTypes::Status::critical));
}

void GroundControl::HandleFaultGaveUp(qint64 sequence)
{
    if (!pendingFaults_.contains(sequence))
        return;

    const PendingFault fault = pendingFaults_.take(sequence);
    faultsModel_.UpdateRow(fault.row, "No Response", static_cast<int>(SharedTypes::Status::critical));
}