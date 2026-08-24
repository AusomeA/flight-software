#include "FlightComputerUI.h"
#include "ReadoutFormatting.h"
#include "Helpers.h"
#include "TelemetryJson.h"

FlightComputerUI::FlightComputerUI(QObject *parent)
    : QObject(parent),
    receiver_(SharedTypes::telemetryPort)
{
    PopulateRows();
    connect(&receiver_, &UdpReceiver::DatagramReceived, this, &FlightComputerUI::HandleTelemetry);
}

void FlightComputerUI::PopulateRows()
{
    readoutsModel_.SetRows({
        {"Mode", "", 0},
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
    });
}

void FlightComputerUI::UpdateRows(){

    const SharedTypes::Telemetry &telemetry = flightComputer_.GetTelemetry();

    readoutsModel_.UpdateRow(modeRow, flightComputer_.GetMode() == SharedTypes::Mode::nominal ? "Nominal" : flightComputer_.GetMode() == SharedTypes::Mode::degraded ? "Degraded"
                                                                                                                             : "Safe",
                             static_cast<int>(flightComputer_.GetMode()));
    readoutsModel_.UpdateRow(METRow, MissionElapsedTimeText(telemetry.missionElapsedTimeSeconds), static_cast<int>(SharedTypes::Status::none));
    readoutsModel_.UpdateRow(batteryRow, QString("%1 %").arg(telemetry.batteryPercent, 0, 'f', 1), static_cast<int>(GetBatteryStatus(telemetry.batteryPercent)));
    readoutsModel_.UpdateRow(solarGenerationRow, QString("%1 W").arg(telemetry.solarGenerationWatts, 0, 'f', 1), static_cast<int>(GetSolarGenerationStatus(telemetry.solarGenerationWatts, telemetry.isInSunlight)));
    readoutsModel_.UpdateRow(powerConsumptionRow, QString("%1 W").arg(telemetry.powerConsumptionWatts, 0, 'f', 1), static_cast<int>(GetPowerConsumptionStatus(telemetry.powerConsumptionWatts)));
    readoutsModel_.UpdateRow(temperatureRow, QString("%1 C").arg(telemetry.temperatureCelsius, 0, 'f', 1), static_cast<int>(GetTemperatureStatus(telemetry.temperatureCelsius)));
    readoutsModel_.UpdateRow(heaterRow, telemetry.heaterEnabled ? "On" : "Off", static_cast<int>(SharedTypes::Status::none));
    readoutsModel_.UpdateRow(radiatorRow, telemetry.radiatorLouversOpen ? "Open" : "Shut", static_cast<int>(SharedTypes::Status::none));
    readoutsModel_.UpdateRow(sunlightRow, telemetry.isInSunlight ? "Yes" : "No", static_cast<int>(telemetry.isInSunlight ? SharedTypes::Status::good : SharedTypes::Status::none));
    readoutsModel_.UpdateRow(commsRow, telemetry.communicationsAvailable ? "Yes" : "No", static_cast<int>(telemetry.communicationsAvailable ? SharedTypes::Status::good : SharedTypes::Status::none));
    readoutsModel_.UpdateRow(commsTransmittingRow, telemetry.commsTransmitting ? "Yes" : "No", static_cast<int>(telemetry.commsTransmitting ? SharedTypes::Status::good : SharedTypes::Status::none));
    readoutsModel_.UpdateRow(temperatureSensorRow, telemetry.temperatureSensorHealthy ? "Yes" : "No", static_cast<int>(telemetry.temperatureSensorHealthy ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(powerSensorRow, telemetry.powerSensorHealthy ? "Yes" : "No", static_cast<int>(telemetry.powerSensorHealthy ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(attitudeSensorRow, telemetry.attitudeSensorHealthy ? "Yes" : "No", static_cast<int>(telemetry.attitudeSensorHealthy ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(payloadRow, telemetry.payloadEnabled ? "Yes" : "No", static_cast<int>(telemetry.payloadEnabled ? SharedTypes::Status::good : SharedTypes::Status::none));
}

void FlightComputerUI::HandleTelemetry(const QByteArray &payload)
{
    SharedTypes::Telemetry telemetry = TelemetryFromJson(payload);
    flightComputer_.Update(telemetry);
    UpdateRows();
}