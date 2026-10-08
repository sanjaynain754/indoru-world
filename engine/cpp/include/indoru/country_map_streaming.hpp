#pragma once

#include "indoru/globe_projection.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace indoru::streaming {

enum class SliceKind : std::uint8_t {
    CapitalCore,
    ResourceFrontier,
    WaterCorridor,
    RemoteSettlements
};

struct CountryAnchor {
    std::string countryId;
    globe::LatLon center{};
    float mapExtentKm{100.0F};
};

struct SliceRequest {
    std::string countryId;
    SliceKind kind{SliceKind::CapitalCore};
    std::uint8_t priority{0};
};

struct SliceState {
    SliceRequest request{};
    bool loaded{false};
    bool requested{false};
};

class CountryMapStreaming final {
public:
    explicit CountryMapStreaming(std::uint8_t maxResidentSlices = 4);

    bool register_country(CountryAnchor anchor);
    bool request_from_globe_pin(globe::LatLon pin, std::uint8_t priority = 255);
    bool request_slice(std::string countryId, SliceKind kind, std::uint8_t priority);
    void advance_frame(std::uint32_t frameBudget);
    void unload_country(std::string_view countryId);

    [[nodiscard]] std::string country_for_pin(globe::LatLon pin) const;
    [[nodiscard]] std::vector<SliceState> states() const;
    [[nodiscard]] std::uint8_t max_resident_slices() const noexcept;
    [[nodiscard]] std::uint8_t resident_slice_count() const noexcept;

private:
    std::vector<CountryAnchor> countries_;
    std::vector<SliceState> slices_;
    std::uint8_t maxResidentSlices_{4};
    std::uint8_t residentSlices_{0};
};

} // namespace indoru::streaming
