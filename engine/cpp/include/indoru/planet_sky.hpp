#pragma once

#include "indoru/planet_atmosphere.hpp"
#include "indoru/planet_day_night.hpp"

#include <cstdint>

namespace indoru::sky {

// Renderer-facing lighting contract.
//
// This module never re-derives solar geometry: it consumes a planet::SunState
// produced by the single planet clock and a local atmosphere::State, then turns
// them into sky colours, sun and moon irradiance, fog, star visibility and
// shadow parameters for the renderer. A renderer that consumes this state keeps
// terrain, weather, ocean and lighting synchronized on one time model.
struct Config final {
    // Deterministic night-sky hue variation so worlds can share the same code.
    std::uint64_t seed{0x1D0A2026ULL};
    double solarIrradianceWattsPerSquareMeter{1'050.0};
    double moonIrradianceWattsPerSquareMeter{0.30};
    double synodicMonthSeconds{2'551'442.9};
    double lunarOrbitalInclinationDegrees{5.145};
    double starBrightness{0.90};
    double zenithScatter{0.72};
    double horizonHaze{0.55};
    double fogBaseDensity{0.0016};
    double fogScaleHeightMeters{8'400.0};
    double maximumShadowSoftness{1.0};
};

struct Forcing final {
    planet::SunState sun{};
    atmosphere::State air{};
    double simulationSeconds{0.0};
    double observerAltitudeMeters{0.0};
    double weatherIntensity{0.0};
};

// Linear RGB in the 0..1 range; the renderer applies its own tone mapping.
struct Color final {
    double red{0.0};
    double green{0.0};
    double blue{0.0};
};

struct MoonState final {
    // 0.0 = new moon, 0.5 = full moon, 1.0 = new moon again.
    double phase{0.0};
    double illumination{0.0};
    double elevationDegrees{0.0};
    double azimuthDegrees{0.0};
    // Unit vector from the surface point toward the moon.
    globe::Vec3 direction{};
    double intensity{0.0};
};

struct ShadowState final {
    // Unit vector from the surface point toward the light source.
    globe::Vec3 lightDirection{};
    // Ground-projected shadow length multiplier; 0 when the sun is at or below
    // the horizon and shadows must be disabled.
    double lengthFactor{0.0};
    double softness{0.0};
    double intensity{0.0};
};

struct State final {
    double dayNightBlend{0.0};
    double sunElevationDegrees{-90.0};
    double sunAzimuthDegrees{0.0};
    globe::Vec3 sunDirection{};
    double sunIntensity{0.0};
    Color sunColor{};
    Color skyZenith{};
    Color skyHorizon{};
    Color ambientColor{};
    Color fogColor{};
    double fogDensity{0.0};
    double starVisibility{0.0};
    MoonState moon{};
    ShadowState shadow{};
    // Bounded exposure multiplier hint: 1.0 in full daylight, larger at night.
    double exposureHint{1.0};
};

[[nodiscard]] bool valid_config(const Config& config) noexcept;
[[nodiscard]] bool valid_forcing(const Forcing& forcing) noexcept;

// Invalid config, forcing or location returns a safe default state rather than
// propagating NaN values into the renderer.
[[nodiscard]] State sample(const Config& config,
                           const Forcing& forcing,
                           globe::LatLon location) noexcept;

} // namespace indoru::sky
