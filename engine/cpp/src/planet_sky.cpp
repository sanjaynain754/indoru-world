#include "indoru/planet_sky.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::sky {
namespace {
constexpr double Pi = 3.14159265358979323846;
constexpr double FullTurn = 360.0;
constexpr double DegreesToRadians = Pi / 180.0;
constexpr double ReferenceAirMass = 38.0;

bool finite_non_negative(double value) noexcept {
    return std::isfinite(value) && value >= 0.0;
}

double clamp(double value, double low, double high) noexcept {
    return std::max(low, std::min(high, value));
}

double saturate(double value) noexcept { return clamp(value, 0.0, 1.0); }
double to_radians(double value) noexcept { return value * DegreesToRadians; }
double to_degrees(double value) noexcept { return value / DegreesToRadians; }

double wrap_degrees(double value) noexcept {
    value = std::fmod(value + 180.0, FullTurn);
    if (value < 0.0) value += FullTurn;
    return value - 180.0;
}

double mix(double low, double high, double alpha) noexcept { return low + (high - low) * alpha; }

Color mix_color(Color low, Color high, double alpha) noexcept {
    return {mix(low.red, high.red, alpha), mix(low.green, high.green, alpha), mix(low.blue, high.blue, alpha)};
}

Color scale_color(Color color, double factor) noexcept {
    return {color.red * factor, color.green * factor, color.blue * factor};
}

Color add_color(Color base, Color extra) noexcept {
    return {base.red + extra.red, base.green + extra.green, base.blue + extra.blue};
}

Color bounded_color(Color color) noexcept {
    return {saturate(color.red), saturate(color.green), saturate(color.blue)};
}

globe::Vec3 normalize_vector(globe::Vec3 vector) noexcept {
    const double length = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return {vector.x / length, vector.y / length, vector.z / length};
}

globe::Vec3 cross_product(globe::Vec3 first, globe::Vec3 second) noexcept {
    return {first.y * second.z - first.z * second.y,
            first.z * second.x - first.x * second.z,
            first.x * second.y - first.y * second.x};
}

struct Basis final {
    globe::Vec3 up{};
    globe::Vec3 east{};
    globe::Vec3 north{};
};

// East/north/up basis in the globe frame. Y is the polar axis, so east is the
// up/pole cross product and north closes the right-handed frame.
Basis basis_at(globe::LatLon location) noexcept {
    Basis basis;
    basis.up = globe::to_unit_sphere(location);
    const globe::Vec3 poleAxis{0.0, 1.0, 0.0};
    globe::Vec3 east = cross_product(basis.up, poleAxis);
    const double eastLength = std::sqrt(east.x * east.x + east.y * east.y + east.z * east.z);
    if (eastLength <= 1e-9) east = cross_product(basis.up, globe::Vec3{1.0, 0.0, 0.0});
    basis.east = normalize_vector(east);
    basis.north = normalize_vector(cross_product(basis.east, basis.up));
    return basis;
}

// Meteorological elevation and azimuth (clockwise from north) to a globe-frame unit vector.
globe::Vec3 direction_from(const Basis& basis, double elevationDegrees, double azimuthDegrees) noexcept {
    const double elevation = to_radians(elevationDegrees);
    const double azimuth = to_radians(azimuthDegrees);
    const double horizontal = std::cos(elevation);
    const double northComponent = horizontal * std::cos(azimuth);
    const double eastComponent = horizontal * std::sin(azimuth);
    const double upComponent = std::sin(elevation);
    return normalize_vector({
        basis.up.x * upComponent + basis.north.x * northComponent + basis.east.x * eastComponent,
        basis.up.y * upComponent + basis.north.y * northComponent + basis.east.y * eastComponent,
        basis.up.z * upComponent + basis.north.z * northComponent + basis.east.z * eastComponent,
    });
}

struct Direction final {
    double elevationDegrees{0.0};
    double azimuthDegrees{0.0};
};

// Same equatorial-to-horizontal convention as indoru::planet::sample_sun, so the
// moon shares the sun's geometry instead of a second spherical model.
Direction horizontal_direction(double latitudeDegrees, double declinationDegrees, double hourAngleDegrees) noexcept {
    const double latitude = to_radians(latitudeDegrees);
    const double declination = to_radians(declinationDegrees);
    const double hourAngle = to_radians(hourAngleDegrees);
    const double sineElevation = std::sin(latitude) * std::sin(declination) +
                                 std::cos(latitude) * std::cos(declination) * std::cos(hourAngle);
    Direction direction;
    direction.elevationDegrees = to_degrees(std::asin(clamp(sineElevation, -1.0, 1.0)));
    const double numerator = -std::sin(hourAngle) * std::cos(declination);
    const double denominator = std::cos(latitude) * std::sin(declination) -
                               std::sin(latitude) * std::cos(declination) * std::cos(hourAngle);
    direction.azimuthDegrees = wrap_degrees(to_degrees(std::atan2(numerator, denominator)));
    return direction;
}

struct AirView final {
    double cloud{0.0};
    double humidity{0.5};
};

// The atmosphere state is optional: a missing or malformed field falls back to
// clear-sky values instead of invalidating the whole lighting sample.
AirView air_view(const atmosphere::State& air) noexcept {
    AirView view;
    if (std::isfinite(air.cloudFactor)) view.cloud = saturate(air.cloudFactor);
    if (std::isfinite(air.relativeHumidity)) view.humidity = saturate(air.relativeHumidity);
    return view;
}
} // namespace

bool valid_config(const Config& config) noexcept {
    return finite_non_negative(config.solarIrradianceWattsPerSquareMeter) &&
           finite_non_negative(config.moonIrradianceWattsPerSquareMeter) &&
           std::isfinite(config.synodicMonthSeconds) && config.synodicMonthSeconds > 0.0 &&
           std::isfinite(config.lunarOrbitalInclinationDegrees) && config.lunarOrbitalInclinationDegrees >= 0.0 &&
           config.lunarOrbitalInclinationDegrees <= 90.0 && std::isfinite(config.starBrightness) &&
           config.starBrightness >= 0.0 && config.starBrightness <= 1.0 && std::isfinite(config.zenithScatter) &&
           config.zenithScatter >= 0.0 && config.zenithScatter <= 2.0 && std::isfinite(config.horizonHaze) &&
           config.horizonHaze >= 0.0 && config.horizonHaze <= 2.0 && finite_non_negative(config.fogBaseDensity) &&
           std::isfinite(config.fogScaleHeightMeters) && config.fogScaleHeightMeters > 0.0 &&
           finite_non_negative(config.maximumShadowSoftness);
}

bool valid_forcing(const Forcing& forcing) noexcept {
    return std::isfinite(forcing.sun.solarElevationDegrees) && std::isfinite(forcing.sun.solarAzimuthDegrees) &&
           std::isfinite(forcing.sun.subsolarLongitudeDegrees) && std::isfinite(forcing.sun.declinationDegrees) &&
           std::isfinite(forcing.sun.daylightFactor) && forcing.sun.daylightFactor >= 0.0 &&
           forcing.sun.daylightFactor <= 1.0 && std::isfinite(forcing.simulationSeconds) &&
           std::isfinite(forcing.observerAltitudeMeters) && std::isfinite(forcing.weatherIntensity) &&
           forcing.weatherIntensity >= 0.0 && forcing.weatherIntensity <= 1.0;
}

State sample(const Config& config, const Forcing& forcing, globe::LatLon location) noexcept {
    State state;
    if (!valid_config(config) || !valid_forcing(forcing) || !globe::valid(location)) return state;
    location = globe::normalize(location);

    const Basis basis = basis_at(location);
    const AirView air = air_view(forcing.air);
    const double cloud = air.cloud;
    const double elevationDegrees = forcing.sun.solarElevationDegrees;
    const double elevationRadians = to_radians(elevationDegrees);
    const double sineElevation = std::sin(elevationRadians);
    const bool aboveHorizon = sineElevation > 0.0;

    // Consumes the day-night model's own blend instead of recomputing twilight.
    state.dayNightBlend = saturate(forcing.sun.daylightFactor);
    state.sunElevationDegrees = elevationDegrees;
    state.sunAzimuthDegrees = forcing.sun.solarAzimuthDegrees;
    state.sunDirection = direction_from(basis, elevationDegrees, forcing.sun.solarAzimuthDegrees);

    const double airMass = aboveHorizon ? clamp(1.0 / std::max(sineElevation, 0.035), 1.0, ReferenceAirMass)
                                        : ReferenceAirMass;
    const double reddening = saturate((airMass - 1.0) / 24.0);
    const double cloudAttenuation = 1.0 - 0.72 * cloud;
    const double direct = std::pow(saturate(sineElevation), 0.55);
    const double twilight = forcing.sun.civilTwilight ? saturate(1.0 - std::abs(elevationDegrees) / 6.0) * 0.02 : 0.0;
    state.sunIntensity = config.solarIrradianceWattsPerSquareMeter * std::max(direct, twilight) * cloudAttenuation;
    state.sunColor = bounded_color(mix_color({1.0, 0.96, 0.90}, {1.0, 0.52, 0.22}, reddening));

    const double blend = std::pow(state.dayNightBlend, 0.65);
    const double seedTint = static_cast<double>(config.seed % 1'000U) * 0.0002;
    const Color nightZenith{0.010 + seedTint * 0.004, 0.014 + seedTint * 0.002, 0.038 + seedTint * 0.010};
    const Color dayZenith = scale_color({0.22, 0.50, 1.08}, config.zenithScatter);
    Color zenith = mix_color(nightZenith, dayZenith, blend);
    zenith = mix_color(zenith, {0.30, 0.32, 0.35}, cloud * 0.55 * state.dayNightBlend);
    state.skyZenith = bounded_color(zenith);

    const Color nightHorizon{0.020 + seedTint * 0.006, 0.026 + seedTint * 0.003, 0.060 + seedTint * 0.014};
    const Color dayHorizon = scale_color({1.13, 1.35, 1.67}, config.horizonHaze);
    const double sunsetWeight = std::exp(-std::pow(elevationDegrees / 9.0, 2.0)) * (1.0 - cloud * 0.55);
    Color horizon = mix_color(nightHorizon, dayHorizon, blend);
    horizon = add_color(horizon, scale_color({0.95, 0.50, 0.24}, sunsetWeight * 0.55 * (1.0 - blend * 0.35)));
    state.skyHorizon = bounded_color(horizon);

    const Color nightAmbient{0.030, 0.040, 0.075};
    const Color dayAmbient = scale_color({0.52, 0.60, 0.74}, 0.62 * (1.0 - 0.25 * cloud));
    state.ambientColor = bounded_color(add_color(mix_color(nightAmbient, dayAmbient, blend),
                                                 scale_color({0.10, 0.10, 0.11}, cloud * state.dayNightBlend)));

    state.fogColor = bounded_color(mix_color(state.skyHorizon, state.skyZenith, 0.35));
    const double observerAltitude = std::max(0.0, forcing.observerAltitudeMeters);
    state.fogDensity = clamp(config.fogBaseDensity * (0.35 + 0.65 * cloud + 0.5 * air.humidity) *
                                 std::exp(-observerAltitude / config.fogScaleHeightMeters),
                             0.0, 0.08);

    state.starVisibility = saturate((1.0 - saturate(state.dayNightBlend * 1.6)) * (1.0 - cloud) * config.starBrightness);

    // The moon is a phase-shifted view of the same clock: the synodic month
    // drives its elongation from the sun, so there is no second time source.
    const double monthProgress =
        std::fmod(std::max(0.0, forcing.simulationSeconds), config.synodicMonthSeconds) / config.synodicMonthSeconds;
    state.moon.phase = monthProgress;
    state.moon.illumination = 0.5 * (1.0 - std::cos(2.0 * Pi * monthProgress));
    const double moonSubsolarLongitude = wrap_degrees(forcing.sun.subsolarLongitudeDegrees + monthProgress * FullTurn);
    const double moonHourAngle = globe::shortest_longitude_delta(moonSubsolarLongitude, location.longitudeDegrees);
    const double moonDeclination = to_degrees(std::asin(clamp(
        std::sin(to_radians(config.lunarOrbitalInclinationDegrees)) * std::sin(2.0 * Pi * monthProgress), -1.0, 1.0)));
    const Direction moonDirection = horizontal_direction(location.latitudeDegrees, moonDeclination, moonHourAngle);
    state.moon.elevationDegrees = moonDirection.elevationDegrees;
    state.moon.azimuthDegrees = moonDirection.azimuthDegrees;
    state.moon.direction = direction_from(basis, moonDirection.elevationDegrees, moonDirection.azimuthDegrees);
    state.moon.intensity = state.moon.elevationDegrees > 0.0
                               ? config.moonIrradianceWattsPerSquareMeter * state.moon.illumination *
                                     (1.0 - state.dayNightBlend)
                               : 0.0;

    const double above = saturate(sineElevation);
    state.shadow.lightDirection = state.sunDirection;
    state.shadow.lengthFactor = elevationDegrees > 0.5
                                    ? clamp(1.0 / std::max(std::tan(elevationRadians), 0.05), 0.0, 60.0)
                                    : 0.0;
    state.shadow.softness =
        clamp(std::pow(1.0 - above, 0.7) * 0.55 + cloud * 0.45, 0.0, 1.0) * config.maximumShadowSoftness;
    state.shadow.intensity = saturate(state.dayNightBlend * (1.0 - 0.7 * cloud));
    state.exposureHint = clamp(1.0 / (0.22 + 0.78 * state.dayNightBlend), 1.0, 5.0);
    return state;
}

} // namespace indoru::sky
