#include "indoru/planet_sky.hpp"

#include <cassert>
#include <cmath>

namespace {

bool bounded_color(indoru::sky::Color color) {
    return color.red >= 0.0 && color.red <= 1.0 && color.green >= 0.0 && color.green <= 1.0 &&
           color.blue >= 0.0 && color.blue <= 1.0;
}

double dot(indoru::globe::Vec3 first, indoru::globe::Vec3 second) {
    return first.x * second.x + first.y * second.y + first.z * second.z;
}

double length(indoru::globe::Vec3 vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

bool identical(const indoru::sky::State& first, const indoru::sky::State& second) {
    return first.dayNightBlend == second.dayNightBlend && first.sunIntensity == second.sunIntensity &&
           first.fogDensity == second.fogDensity && first.starVisibility == second.starVisibility &&
           first.sunDirection.x == second.sunDirection.x && first.sunDirection.y == second.sunDirection.y &&
           first.moon.phase == second.moon.phase && first.moon.illumination == second.moon.illumination &&
           first.shadow.lengthFactor == second.shadow.lengthFactor && first.exposureHint == second.exposureHint;
}
} // namespace

int main() {
    using namespace indoru;

    sky::Config config{};
    assert(sky::valid_config(config));
    assert(!sky::valid_config(sky::Config{0x1U, -1.0}));
    assert(!sky::valid_config(sky::Config{0x1U, 1'000.0, 0.3, 0.0}));
    assert(!sky::valid_config(sky::Config{0x1U, 1'000.0, 0.3, 2'551'442.9, 5.145, 1.5}));

    planet::Clock clock{};
    sky::Forcing noon{};
    noon.sun = planet::sample_sun({0.0, 0.0}, clock);
    assert(sky::valid_forcing(noon));

    auto broken = noon;
    broken.sun.daylightFactor = 1.5;
    assert(!sky::valid_forcing(broken));
    broken = noon;
    broken.sun.solarElevationDegrees = std::nan("");
    assert(!sky::valid_forcing(broken));
    broken = noon;
    broken.weatherIntensity = 1.5;
    assert(!sky::valid_forcing(broken));

    const auto dayState = sky::sample(config, noon, {0.0, 0.0});
    assert(dayState.dayNightBlend > 0.99);
    assert(dayState.sunIntensity > 800.0);
    assert(dayState.starVisibility < 0.05);
    assert(dayState.shadow.intensity > 0.5);
    assert(dayState.shadow.lengthFactor < 0.2);
    assert(dayState.exposureHint < 1.001);
    assert(dayState.fogDensity >= 0.0 && dayState.fogDensity <= 0.08);
    assert(bounded_color(dayState.sunColor));
    assert(bounded_color(dayState.skyZenith));
    assert(bounded_color(dayState.skyHorizon));
    assert(bounded_color(dayState.ambientColor));
    assert(bounded_color(dayState.fogColor));
    assert(std::abs(length(dayState.sunDirection) - 1.0) < 1e-9);
    assert(std::abs(length(dayState.shadow.lightDirection) - 1.0) < 1e-9);
    const auto up = globe::to_unit_sphere({0.0, 0.0});
    assert(dot(dayState.sunDirection, up) > 0.99);

    planet::Clock midnightClock{};
    midnightClock.simulationSeconds = 43'200.0;
    sky::Forcing midnight{};
    midnight.sun = planet::sample_sun({0.0, 0.0}, midnightClock);
    const auto nightState = sky::sample(config, midnight, {0.0, 0.0});
    assert(nightState.dayNightBlend < 0.01);
    assert(nightState.sunIntensity < 5.0);
    assert(nightState.starVisibility > 0.5);
    assert(nightState.shadow.intensity < 0.01);
    assert(nightState.shadow.lengthFactor == 0.0);
    assert(nightState.exposureHint > 4.0);
    assert(bounded_color(nightState.skyZenith));
    assert(bounded_color(nightState.skyHorizon));
    assert(bounded_color(nightState.fogColor));

    const auto rejected = sky::sample(config, noon, {91.0, 0.0});
    assert(rejected.sunIntensity == 0.0);
    assert(rejected.fogDensity == 0.0);
    assert(rejected.starVisibility == 0.0);
    assert(!sky::valid_forcing(broken) && sky::sample(sky::Config{0x1U, -1.0}, noon, {0.0, 0.0}).sunIntensity == 0.0);

    assert(identical(dayState, sky::sample(config, noon, {0.0, 0.0})));

    // Full moon is opposite the sun: the synodic phase drives the elongation.
    planet::Clock fullMoonClock{};
    fullMoonClock.simulationSeconds = config.synodicMonthSeconds * 0.5;
    sky::Forcing fullMoon{};
    fullMoon.sun = planet::sample_sun({0.0, 0.0}, fullMoonClock);
    fullMoon.simulationSeconds = fullMoonClock.simulationSeconds;
    const auto fullMoonState = sky::sample(config, fullMoon, {0.0, 0.0});
    assert(std::abs(fullMoonState.moon.phase - 0.5) < 1e-9);
    assert(fullMoonState.moon.illumination > 0.99);
    assert(fullMoon.sun.solarElevationDegrees > 0.0);
    assert(fullMoonState.moon.elevationDegrees < 0.0);
    assert(fullMoonState.moon.intensity == 0.0);
    assert(std::abs(length(fullMoonState.moon.direction) - 1.0) < 1e-9);
    assert(fullMoonState.moon.azimuthDegrees >= -180.0 && fullMoonState.moon.azimuthDegrees <= 180.0);
    return 0;
}
