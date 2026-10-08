#include "indoru/planet_day_night.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::planet {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double FullTurn = 360.0;

double radians(double degrees) noexcept { return degrees * Pi / 180.0; }
double degrees(double radiansValue) noexcept { return radiansValue * 180.0 / Pi; }
double clamp(double value, double low, double high) noexcept { return std::max(low, std::min(high, value)); }
double wrap_longitude(double value) noexcept {
    value = std::fmod(value + 180.0, FullTurn);
    if (value < 0.0) value += FullTurn;
    return value - 180.0;
}
}

bool valid_clock(const Clock& clock) noexcept {
    return std::isfinite(clock.simulationSeconds) && std::isfinite(clock.solarDaySeconds) &&
           std::isfinite(clock.orbitalPeriodSeconds) && std::isfinite(clock.axialTiltDegrees) &&
           clock.solarDaySeconds > 0.0 && clock.orbitalPeriodSeconds > 0.0 &&
           clock.axialTiltDegrees >= 0.0 && clock.axialTiltDegrees <= 90.0;
}

double local_solar_hour(globe::LatLon location, const Clock& clock) noexcept {
    if (!globe::valid(location) || !valid_clock(clock)) return 0.0;
    const double dayProgress = std::fmod(clock.simulationSeconds, clock.solarDaySeconds) / clock.solarDaySeconds;
    const double rotationLongitude = wrap_longitude(dayProgress * FullTurn);
    const double hourAngle = globe::shortest_longitude_delta(rotationLongitude, location.longitudeDegrees);
    return 12.0 + hourAngle / 15.0;
}

SunState sample_sun(globe::LatLon location, const Clock& clock) noexcept {
    SunState state;
    if (!globe::valid(location) || !valid_clock(clock)) return state;
    const double yearProgress = std::fmod(clock.simulationSeconds, clock.orbitalPeriodSeconds) / clock.orbitalPeriodSeconds;
    const double orbitalAngle = yearProgress * 2.0 * Pi;
    state.declinationDegrees = degrees(std::asin(std::sin(radians(clock.axialTiltDegrees)) * std::sin(orbitalAngle)));
    const double dayProgress = std::fmod(clock.simulationSeconds, clock.solarDaySeconds) / clock.solarDaySeconds;
    state.subsolarLongitudeDegrees = wrap_longitude(dayProgress * FullTurn);
    const double latitude = radians(location.latitudeDegrees);
    const double declination = radians(state.declinationDegrees);
    const double hourAngle = radians(globe::shortest_longitude_delta(state.subsolarLongitudeDegrees, location.longitudeDegrees));
    const double sineElevation = std::sin(latitude) * std::sin(declination) + std::cos(latitude) * std::cos(declination) * std::cos(hourAngle);
    state.solarElevationDegrees = degrees(std::asin(clamp(sineElevation, -1.0, 1.0)));
    const double azimuthNumerator = -std::sin(hourAngle) * std::cos(declination);
    const double azimuthDenominator = std::cos(latitude) * std::sin(declination) - std::sin(latitude) * std::cos(declination) * std::cos(hourAngle);
    state.solarAzimuthDegrees = wrap_longitude(degrees(std::atan2(azimuthNumerator, azimuthDenominator)));
    state.daylight = state.solarElevationDegrees > 0.0;
    state.civilTwilight = state.solarElevationDegrees > -6.0;
    state.daylightFactor = clamp((state.solarElevationDegrees + 6.0) / 12.0, 0.0, 1.0);
    return state;
}

} // namespace indoru::planet
