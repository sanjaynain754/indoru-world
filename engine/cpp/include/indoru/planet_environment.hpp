#pragma once

#include "indoru/planet_atmosphere.hpp"
#include "indoru/planet_day_night.hpp"
#include "indoru/planet_ocean.hpp"
#include "indoru/planet_sky.hpp"
#include "indoru/planet_terrain.hpp"

namespace indoru::environment {

// Single entry point for the runtime and streaming layers.
//
// One call at one location and one simulation time returns gravity, ground,
// atmosphere, sun, sky and water derived from the same clock, so no subsystem
// has to invent its own time or forcing model. Region branches override the
// per-system Config values instead of forking the sampling pipeline.
struct Config final {
    planet::Clock clock{};
    terrain::Config terrain{};
    atmosphere::Config atmosphere{};
    ocean::Config ocean{};
    sky::Config sky{};
    double waveFetchMeters{250'000.0};
    double swellHeightMeters{1.2};
    double swellPeriodSeconds{11.0};
    // Extra seabed depth below the terrain surface. The procedural terrain
    // already encodes bathymetry in its elevation, so this stays 0 unless a
    // region streamer supplies a separate seabed.
    double seabedDepthOffsetMeters{0.0};
};

struct Forcing final {
    double simulationSeconds{0.0};
    double weatherIntensity{0.0};
    double tidePhaseRadians{0.0};
};

struct State final {
    globe::LatLon location{};
    // False when the inputs were rejected; every other field is then zeroed.
    bool valid{false};
    earth::GravitySample gravity{};
    terrain::Sample ground{};
    atmosphere::State air{};
    planet::SunState sun{};
    sky::State sky{};
    ocean::OceanState water{};
};

[[nodiscard]] bool valid_config(const Config& config) noexcept;
[[nodiscard]] bool valid_forcing(const Forcing& forcing) noexcept;
[[nodiscard]] State sample(const Config& config, const Forcing& forcing, globe::LatLon location) noexcept;

} // namespace indoru::environment
