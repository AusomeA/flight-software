#include "FlightComputerUI.h"
#include "ReadoutFormatting.h"
#include "Helpers.h"
#include "TelemetryJson.h"
#include <QCoreApplication>

FlightComputerUI::FlightComputerUI(QObject *parent)
    : QObject(parent),
      receiver_(SharedTypes::telemetryPort),
      simulatorAddress_(QHostAddress::LocalHost)
{
    PopulateRows();
    connect(&receiver_, &UdpReceiver::DatagramReceived, this, &FlightComputerUI::HandleTelemetry);

    timeSinceLastPacket_.start();
    connect(&linkCheckTimer_, &QTimer::timeout, this, &FlightComputerUI::UpdateLinkRow);
    linkCheckTimer_.start(linkCheckIntervalMilliseconds);

    const QStringList arguments = QCoreApplication::arguments();
    if(arguments.size() > 1)
    {
        simulatorAddress_ = QHostAddress(arguments[1]);
        if(simulatorAddress_.isNull())
            qFatal("Invalid address argument: %s", qPrintable(arguments[1]));
    }
}

void FlightComputerUI::PopulateRows()
{
    readoutsModel_.SetRows({
        {"Link", "", 0},
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

void FlightComputerUI::UpdateRows(bool stale)
{
    const SharedTypes::Telemetry &telemetry = flightComputer_.GetTelemetry();

    auto rowStatus = [stale] (SharedTypes::Status status)                           // helper lambda
    { return static_cast<int>(stale ? SharedTypes::Status::stale : status);};
    
    readoutsModel_.UpdateRow(modeRow, flightComputer_.GetMode() == SharedTypes::Mode::nominal ? "Nominal" : flightComputer_.GetMode() == SharedTypes::Mode::degraded ? "Degraded" : "Safe", 
                                        rowStatus(flightComputer_.GetMode() == SharedTypes::Mode::nominal ? SharedTypes::Status::good 
                                            : flightComputer_.GetMode() == SharedTypes::Mode::degraded ? SharedTypes::Status::warning
                                            : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(METRow, MissionElapsedTimeText(telemetry.missionElapsedTimeSeconds), rowStatus(SharedTypes::Status::none));
    readoutsModel_.UpdateRow(batteryRow, QString("%1 %").arg(telemetry.batteryPercent, 0, 'f', 1), rowStatus(GetBatteryStatus(telemetry.batteryPercent)));
    readoutsModel_.UpdateRow(solarGenerationRow, QString("%1 W").arg(telemetry.solarGenerationWatts, 0, 'f', 1), rowStatus(GetSolarGenerationStatus(telemetry.solarGenerationWatts, telemetry.isInSunlight)));
    readoutsModel_.UpdateRow(powerConsumptionRow, QString("%1 W").arg(telemetry.powerConsumptionWatts, 0, 'f', 1), rowStatus(GetPowerConsumptionStatus(telemetry.powerConsumptionWatts)));
    readoutsModel_.UpdateRow(temperatureRow, QString("%1 C").arg(telemetry.temperatureCelsius, 0, 'f', 1), rowStatus(GetTemperatureStatus(telemetry.temperatureCelsius)));
    readoutsModel_.UpdateRow(heaterRow, telemetry.heaterEnabled ? "On" : "Off", rowStatus(SharedTypes::Status::none));
    readoutsModel_.UpdateRow(radiatorRow, telemetry.radiatorLouversOpen ? "Open" : "Shut", rowStatus(SharedTypes::Status::none));
    readoutsModel_.UpdateRow(sunlightRow, telemetry.isInSunlight ? "Yes" : "No", rowStatus(telemetry.isInSunlight ? SharedTypes::Status::good : SharedTypes::Status::none));
    readoutsModel_.UpdateRow(commsRow, telemetry.communicationsAvailable ? "Yes" : "No", rowStatus(telemetry.communicationsAvailable ? SharedTypes::Status::good : SharedTypes::Status::none));
    readoutsModel_.UpdateRow(commsTransmittingRow, telemetry.commsTransmitting ? "Yes" : "No", rowStatus(telemetry.commsTransmitting ? SharedTypes::Status::good : SharedTypes::Status::none));
    readoutsModel_.UpdateRow(temperatureSensorRow, telemetry.temperatureSensorHealthy ? "Yes" : "No", rowStatus(telemetry.temperatureSensorHealthy ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(powerSensorRow, telemetry.powerSensorHealthy ? "Yes" : "No", rowStatus(telemetry.powerSensorHealthy ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(attitudeSensorRow, telemetry.attitudeSensorHealthy ? "Yes" : "No", rowStatus(telemetry.attitudeSensorHealthy ? SharedTypes::Status::good : SharedTypes::Status::critical));
    readoutsModel_.UpdateRow(payloadRow, telemetry.payloadEnabled ? "Yes" : "No", rowStatus(telemetry.payloadEnabled ? SharedTypes::Status::good : SharedTypes::Status::none));
}

void FlightComputerUI::HandleTelemetry(const QByteArray &payload)
{
    std::optional<SharedTypes::Telemetry> telemetry = TelemetryFromJson(payload);
    if(!telemetry)
    {
        qWarning() << "Dropped malformed telemetry packet";
        return;
    }
    timeSinceLastPacket_.restart();
    SharedTypes::Commands commands = flightComputer_.Update(*telemetry);
    commandSocket_.writeDatagram(CommandsToJson(commands), simulatorAddress_, SharedTypes::commandPort);         // Local Host will change later
    UpdateRows();
}

void FlightComputerUI::UpdateLinkRow()
{
    const qint64 silentMillisecond = timeSinceLastPacket_.elapsed();
    const bool linkLost = silentMillisecond > linkLostMilliseconds;

    readoutsModel_.UpdateRow(linkRow, linkLost ? QString("No Link (%1 s)").arg(silentMillisecond / 1000.0, 0, 'f', 1) : "Good Link", static_cast<int>(linkLost ? SharedTypes::Status::critical : SharedTypes::Status::good));

    if(linkLost)
        UpdateRows(true);
}