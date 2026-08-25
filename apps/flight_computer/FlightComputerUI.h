#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QElapsedTimer>
#include <QUdpSocket>
#include "FlightComputer.h"
#include "ReadoutsModel.h"
#include "UdpReceiver.h"

enum FCReadoutRowIndex
{
    linkRow,
    modeRow,
    METRow,
    batteryRow,
    solarGenerationRow,
    powerConsumptionRow,
    temperatureRow,
    heaterRow,
    radiatorRow,
    sunlightRow,
    commsRow,
    commsTransmittingRow,
    temperatureSensorRow,
    powerSensorRow,
    attitudeSensorRow,
    payloadRow,
    readoutRowCount
};

class FlightComputerUI : public QObject {
Q_OBJECT
    QML_NAMED_ELEMENT(FlightComputerUI)
    Q_PROPERTY(QAbstractItemModel *readoutsModel READ ReadoutsModelPtr CONSTANT)

    public:
    FlightComputerUI(QObject *parent = nullptr);

    QAbstractItemModel *ReadoutsModelPtr() {return &readoutsModel_;}

    Q_INVOKABLE void ModeTestUp()   {flightComputer_.ModeTestUp(); UpdateRows();}
    Q_INVOKABLE void ModeTestDown() {flightComputer_.ModeTestDown(); UpdateRows();}

    private:
    FlightComputer flightComputer_;
    ReadoutsModel readoutsModel_;

    UdpReceiver receiver_;
    QUdpSocket commandSocket_;
    QTimer linkCheckTimer_;
    QElapsedTimer timeSinceLastPacket_;

    static constexpr int linkLostMilliseconds = 3000;
    static constexpr int linkCheckIntervalMilliseconds = 200;

    void HandleTelemetry(const QByteArray &payload);
    void UpdateLinkRow();                                           // Have to update seperate in case packet does not arrive
    
    void PopulateRows();
    void UpdateRows(bool stale = false);
};