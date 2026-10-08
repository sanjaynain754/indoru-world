#pragma once

#include "indoru/weather_simulation.hpp"
#include <cstdint>
#include <vector>

namespace indoru::weather {

enum class Band : std::uint8_t {
    NorthPolar,
    MiddlePolar,
    SouthRedCanyon
};

struct PartitionProfile {
    Band band{Band::MiddlePolar};
    float snowRate{0.0f};
    float dustRate{0.0f};
    float windMetersPerSecond{0.0f};
    float temperatureMinC{-10.0f};
    float temperatureMaxC{20.0f};
};

struct Particle {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float velocityY{0.0f};
    float lifetimeSeconds{0.0f};
    bool dust{false};
};

class SnowDustShowroom final {
public:
    explicit SnowDustShowroom(std::uint32_t maxParticles = 2048);

    void set_partition(PartitionProfile profile);
    void advance(float deltaSeconds, std::uint64_t deterministicTick);

    [[nodiscard]] const PartitionProfile& partition() const noexcept;
    [[nodiscard]] const std::vector<Particle>& particles() const noexcept;
    [[nodiscard]] std::uint32_t active_particle_count() const noexcept;

private:
    void emit(float deltaSeconds, std::uint64_t deterministicTick);
    void integrate(float deltaSeconds);

    PartitionProfile profile_{};
    std::uint32_t maxParticles_{2048};
    std::vector<Particle> particles_;
};

[[nodiscard]] PartitionProfile profile_for(Band band) noexcept;
[[nodiscard]] bool validate_profile(const PartitionProfile& profile) noexcept;

} // namespace indoru::weather
