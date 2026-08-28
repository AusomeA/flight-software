#pragma once
#include <SharedTypes.h>

inline constexpr float minTemperatureGoodCelsius = 10.f;    // what temp is still good (minimum)
inline constexpr float maxTemperatureGoodCelsius = 35.f;    // what temp is still good (maximum)
inline constexpr float minTemperatureWarningCelsius = 5.f;  // what temp is still just a warning (minimum)
inline constexpr float maxTemperatureWarningCelsius = 36.f; // what temp is still just a warning (maximum)

inline SharedTypes::Status GetBatteryStatus(float batteryPercentage)
{
    if (batteryPercentage > 50.f)
        return SharedTypes::Status::good;
    else if (batteryPercentage > 20.f)
        return SharedTypes::Status::warning;
    else
        return SharedTypes::Status::critical;
}

inline SharedTypes::Status GetSolarGenerationStatus(float generationWatts, bool isInSunlight)
{
    if (!isInSunlight)
        return SharedTypes::Status::none;

    if (generationWatts > 30.f)
        return SharedTypes::Status::good;
    else if (generationWatts > 10.f)
        return SharedTypes::Status::warning;
    else
        return SharedTypes::Status::critical;
}

inline SharedTypes::Status GetPowerConsumptionStatus(float powerConsumptionWatts)
{
    if (powerConsumptionWatts < 33.75f) // if ran through entire eclipse, would break even on power consumption
        return SharedTypes::Status::good;
    else if (powerConsumptionWatts < 46.f) // 45 watts is everything running at once. Should not go above 45 watts and should only use 45 watts sparingly.
        return SharedTypes::Status::warning;
    else
        return SharedTypes::Status::critical;
}

inline SharedTypes::Status GetTemperatureStatus(float temperatureCelsius)
{
    if (temperatureCelsius <= maxTemperatureGoodCelsius && temperatureCelsius >= minTemperatureGoodCelsius)
        return SharedTypes::Status::good;
    else if (temperatureCelsius <= maxTemperatureWarningCelsius && temperatureCelsius >= minTemperatureWarningCelsius)
        return SharedTypes::Status::warning;
    else
        return SharedTypes::Status::critical;
}