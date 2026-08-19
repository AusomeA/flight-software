#pragma once

#include <QObject>
#include <QQmlEngine>
#include "FlightComputer.h"
#include "ReadoutsModel.h"

class FlightComputerUI : public QObject {
Q_OBJECT
    QML_NAMED_ELEMENT(FlightComputerUI)
    Q_PROPERTY(QAbstractItemModel *readoutsModel READ ReadoutsModelPtr CONSTANT)

    public:
    FlightComputerUI(QObject *parent = nullptr);

    QAbstractItemModel *ReadoutsModelPtr() {return &readoutsModel_;}

    private:
    FlightComputer flightComputer_;
    ReadoutsModel readoutsModel_;

    void PopulateRows();
};