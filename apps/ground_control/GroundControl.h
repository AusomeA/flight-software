#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QElapsedTimer>
#include "ReadoutsModel.h"
#include "Discovery.h"
#include "UdpReceiver.h"
#include "SharedTypes.h"

enum GCReadoutRowIndex
{
    flightComputerLinkRow,
    simulatorLinkRow,
    modeRow,
    gcHeaderRowCount
};

class GroundControl : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(GroundControl)
    Q_PROPERTY(QAbstractItemModel *readoutsModel READ ReadoutsModelPtr CONSTANT)

    public:
    GroundControl(QObject *parent = nullptr);

    QAbstractItemModel *ReadoutsModelPtr() {return &readoutsModel_;}

    private:
    ReadoutsModel readoutsModel_;
    UdpReceiver telemetryReceiver_;

    Discovery discovery_{SharedTypes::groundControlName, SharedTypes::defaultVehicleName};

    QTimer linkCheckTimer_;
    QElapsedTimer timeSinceLastPacket_;

    SharedTypes::Telemetry telemetry_;
    SharedTypes::Mode mode_ = SharedTypes::Mode::nominal;
    bool simLinkOk_ = false;

    static constexpr int linkCheckIntervalMilliseconds = 200;

    void HandleGroundTelemetry(const QByteArray &payload);
    void UpdateLinkRow();
    void PopulateRows();
    void UpdateRows(bool stale = false);
};