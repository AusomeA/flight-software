#include "FlightComputerUI.h"

FlightComputerUI::FlightComputerUI(QObject *parent)
    : QObject(parent)
{
    PopulateRows();
}

void FlightComputerUI::PopulateRows()
{
    readoutsModel_.SetRows({
        {"Time Scale", "", 0},
        {"Mode", "", 0},
        {"Running", "", 0},
        {"MET", "", 0},
        {"Battery", "", 0},
        {"Solar Generation", "", 0},
        {"Power Consumption", "", 0},
        {"Temperature", "", 0},
        {"Heater", "", 0},
        {"Radiator Louvers", "", 0},
        {"In Sunlight", "", 0},
        {"Comms Available", "", 0},
        {"Comms Transmitting", "", 0},
        {"Temp Sensor OK", "", 0},
        {"Power Sensor OK", "", 0},
        {"Attitude Sensor OK", "", 0},
        {"Payload Enabled", "", 0},
        {"Chaos Enabled", "", 0}
    });
}