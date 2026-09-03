#include "FlightComputerShell.h"
#include "ReadoutFormatting.h"
#include "Helpers.h"
#include "TelemetryJson.h"
#include "EnvelopeJson.h"
#include "TelemetryReadouts.h"
#include <QCoreApplication>
#include <iostream>

FlightComputerShell::FlightComputerShell(QObject *parent)
    : QObject(parent),
      receiver_(SharedTypes::simTelemetryPort),
      groundCommandReceiver_(SharedTypes::groundCommandPort),
      simulatorAddress_(QHostAddress::LocalHost)
{
    PopulateRows();
    connect(&receiver_, &UdpReceiver::DatagramReceived, this, &FlightComputerShell::HandleTelemetry);
    connect(&groundCommandReceiver_, &UdpReceiver::DatagramReceived, this, &FlightComputerShell::HandleGroundCommand);

    connect(&linkCheckTimer_, &QTimer::timeout, this, &FlightComputerShell::UpdateLinkRow);
    linkCheckTimer_.start(linkCheckIntervalMilliseconds);

    const QStringList arguments = QCoreApplication::arguments();
    if(arguments.size() > 1)
    {
        simulatorAddress_ = QHostAddress(arguments[1]);
        if(simulatorAddress_.isNull())
            qFatal("Invalid address argument: %s", qPrintable(arguments[1]));
    }

    connect(&discovery_, &Discovery::peerAppeared, this, [this](const QString &appName, const QHostAddress &address) {
        if (appName == SharedTypes::simulatorName) {
            simulatorAddress_ = address;
        }
    });
}

void FlightComputerShell::PopulateRows()
{
    readoutsModel_.SetRows(QVector<ReadoutRow>{
        {"Link", "", 0},
        {"Mode", "", 0},
    } + TelemetryReadouts());
}

void FlightComputerShell::UpdateRows(bool stale)
{
    const SharedTypes::Mode mode = flightComputer_.GetMode();
    const int modeStatus = static_cast<int>(stale ? SharedTypes::Status::stale : GetModeStatus(mode));
    
    readoutsModel_.UpdateRow(modeRow, ModeText(mode), modeStatus);
    UpdateTelemetryReadouts(readoutsModel_, flightComputer_.GetTelemetry(), fcHeaderRowCount, stale);
}

void FlightComputerShell::HandleTelemetry(const QByteArray &payload)
{
    std::optional<SharedTypes::Telemetry> telemetry = TelemetryFromJson(payload);
    if(!telemetry)
    {
        qWarning() << "Dropped malformed telemetry packet";
        return;
    }
    timeSinceLastPacket_.restart();
    SharedTypes::Commands commands = flightComputer_.Update(*telemetry);
    commandSocket_.writeDatagram(CommandsToJson(commands), simulatorAddress_, SharedTypes::simCommandPort);         // Local Host will change later
    UpdateRows();
}

void FlightComputerShell::HandleGroundCommand(const QByteArray &payload, const QHostAddress &senderAddress, quint16 senderPort)
{
    std::optional<Envelope> envelope = EnvelopeFromJson(payload);
    if(!envelope || envelope->type != SharedTypes::groundCommandMessageType)
    {
        qWarning() << "Dropped malformed ground command packet";
        return;
    }

    const QString command = envelope->body["command"].toString();
    bool accepted = false;

    if(command == SharedTypes::exitSafeModeCommand)
        accepted = flightComputer_.RequestExitSafeMode();

    std::cout << "Ground command " << command.toStdString() << (accepted ? " accepted" : " rejected") << std::endl;

    Envelope ackEnvelope;
    ackEnvelope.type = SharedTypes::ackMessageType;
    ackEnvelope.sequence = envelope->sequence;
    ackEnvelope.body["accepted"] = accepted;
    groundTelemetrySocket_.writeDatagram(EnvelopeToJson(ackEnvelope), senderAddress, senderPort);
}

void FlightComputerShell::UpdateLinkRow()
{
    const bool neverHeard = !timeSinceLastPacket_.isValid();
    const qint64 silentMillisecond = neverHeard ? 0 : timeSinceLastPacket_.elapsed();
    const bool linkLost = neverHeard || silentMillisecond > SharedTypes::linkLostMilliseconds;

    readoutsModel_.UpdateRow(linkRow, 
                            neverHeard ? "No Link" : linkLost ? QString("No Link (%1 s)").arg(silentMillisecond / 1000.0, 0, 'f', 1) : "Good Link", 
                            static_cast<int>(linkLost ? SharedTypes::Status::critical : SharedTypes::Status::good));

    if(linkLost)
        UpdateRows(true);

    SendGroundTelemetry(!linkLost);  
}

void FlightComputerShell::SendGroundTelemetry(bool simLinkOk)
{
    Envelope envelope;
    envelope.type = SharedTypes::groundTelemetryMessageType;
    envelope.sequence = groundSequence_++;
    envelope.body["telemetry"] = TelemetryToJsonObject(flightComputer_.GetTelemetry());
    envelope.body["mode"] = static_cast<int>(flightComputer_.GetMode());
    envelope.body["simLinkOk"] = simLinkOk;

    const QByteArray datagram = EnvelopeToJson(envelope);

    for(const QHostAddress &address : discovery_.LivePeerAddresses(SharedTypes::groundControlName))
    {
        groundTelemetrySocket_.writeDatagram(datagram, address, SharedTypes::groundTelemetryPort);
    }
}