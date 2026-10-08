#include "indoru/weather_partition.hpp"

#include <algorithm>
#include <cmath>

namespace indoru::weather {
namespace {
std::uint64_t mix(std::uint64_t value) noexcept {
    value ^= value >> 30U;
    value *= 0xbf58476d1ce4e5b9ULL;
    value ^= value >> 27U;
    value *= 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
}
float unit(std::uint64_t value) noexcept {
    return static_cast<float>((mix(value) >> 11U) & 0xFFFFFFU) / 16'777'215.0F;
}
}

PartitionProfile profile_for(Band band) noexcept {
    switch (band) {
    case Band::NorthPolar: return {band, 0.92F, 0.02F, 18.0F, -35.0F, 4.0F};
    case Band::MiddlePolar: return {band, 0.38F, 0.12F, 12.0F, -12.0F, 14.0F};
    case Band::SouthRedCanyon: return {band, 0.04F, 0.88F, 21.0F, 2.0F, 46.0F};
    }
    return {};
}

bool validate_profile(const PartitionProfile& profile) noexcept {
    return profile.snowRate >= 0.0F && profile.snowRate <= 1.0F &&
           profile.dustRate >= 0.0F && profile.dustRate <= 1.0F &&
           profile.windMetersPerSecond >= 0.0F && profile.temperatureMinC <= profile.temperatureMaxC;
}

SnowDustShowroom::SnowDustShowroom(std::uint32_t maxParticles)
    : profile_(profile_for(Band::MiddlePolar)), maxParticles_(std::max<std::uint32_t>(1U, maxParticles)) {
    particles_.reserve(maxParticles_);
}

void SnowDustShowroom::set_partition(PartitionProfile profile) {
    if (validate_profile(profile)) profile_ = profile;
}

void SnowDustShowroom::advance(float deltaSeconds, std::uint64_t deterministicTick) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds <= 0.0F) return;
    const float clampedDelta = std::min(deltaSeconds, 0.1F);
    integrate(clampedDelta);
    emit(clampedDelta, deterministicTick);
}

void SnowDustShowroom::emit(float deltaSeconds, std::uint64_t deterministicTick) {
    const float rate = (profile_.snowRate + profile_.dustRate) * 260.0F;
    const auto count = static_cast<std::uint32_t>(rate * deltaSeconds);
    for (std::uint32_t index = 0; index < count && particles_.size() < maxParticles_; ++index) {
        const auto seed = deterministicTick * 1315423911ULL + index * 2654435761ULL;
        const bool dust = unit(seed) > profile_.snowRate / std::max(0.001F, profile_.snowRate + profile_.dustRate);
        particles_.push_back({unit(seed + 1ULL) * 100.0F - 50.0F, 35.0F + unit(seed + 2ULL) * 15.0F, unit(seed + 3ULL) * 100.0F - 50.0F,
                              dust ? -1.5F : -4.0F, 4.0F + unit(seed + 4ULL) * 5.0F, dust});
    }
}

void SnowDustShowroom::integrate(float deltaSeconds) {
    for (auto& particle : particles_) {
        particle.x += profile_.windMetersPerSecond * deltaSeconds * (particle.dust ? 0.16F : 0.04F);
        particle.y += particle.velocityY * deltaSeconds;
        particle.lifetimeSeconds -= deltaSeconds;
    }
    particles_.erase(std::remove_if(particles_.begin(), particles_.end(), [](const Particle& particle) {
        return particle.lifetimeSeconds <= 0.0F || particle.y < -5.0F || std::abs(particle.x) > 80.0F;
    }), particles_.end());
}

const PartitionProfile& SnowDustShowroom::partition() const noexcept { return profile_; }
const std::vector<Particle>& SnowDustShowroom::particles() const noexcept { return particles_; }
std::uint32_t SnowDustShowroom::active_particle_count() const noexcept { return static_cast<std::uint32_t>(particles_.size()); }

} // namespace indoru::weather
