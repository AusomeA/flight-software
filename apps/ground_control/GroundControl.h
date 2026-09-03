#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QElapsedTimer>
#include "ReadoutsModel.h"
#include "Discovery.h"
#include "UdpReceiver.h"
#include "SharedTypes.h"
#include "TelemetryReadouts.h"

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
    Q_PROPERTY(bool flightComputerLinked READ FlightComputerLinked NOTIFY summaryChanged)
    Q_PROPERTY(bool simulatorLinked READ SimulatorLinked NOTIFY summaryChanged)
    Q_PROPERTY(QString modeText READ CurrentModeText NOTIFY summaryChanged)
    Q_PROPERTY(int modeStatus READ CurrentModeStatus NOTIFY summaryChanged)

    public:
    GroundControl(QObject *parent = nullptr);

    QAbstractItemModel *ReadoutsModelPtr() {return &readoutsModel_;}

    bool FlightComputerLinked() const {return flightComputerLinked_;}
    bool SimulatorLinked() const {return flightComputerLinked_ && simLinkOk_;}
    QString CurrentModeText() const {return ModeText(mode_);}
    int CurrentModeStatus() const {return static_cast<int>(flightComputerLinked_ ? GetModeStatus(mode_) : SharedTypes::Status::stale);}

    signals:
    void summaryChanged();

    private:
    ReadoutsModel readoutsModel_;
    UdpReceiver telemetryReceiver_;

    Discovery discovery_{SharedTypes::groundControlName, SharedTypes::defaultVehicleName};

    QTimer linkCheckTimer_;
    QElapsedTimer timeSinceLastPacket_;

    SharedTypes::Telemetry telemetry_;
    SharedTypes::Mode mode_ = SharedTypes::Mode::nominal;
    bool simLinkOk_ = false;
    bool flightComputerLinked_ = false;

    static constexpr int linkCheckIntervalMilliseconds = 200;

    void HandleGroundTelemetry(const QByteArray &payload);
    void UpdateLinkRow();
    void PopulateRows();
    void UpdateRows(bool stale = false);
};