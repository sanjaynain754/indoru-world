#include "indoru/country_map_streaming.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

namespace indoru::streaming {
namespace {
constexpr double Pi = 3.14159265358979323846;

double radians(double degrees) noexcept { return degrees * Pi / 180.0; }
double angular_distance(globe::LatLon a, globe::LatLon b) noexcept {
    const double aLat = radians(a.latitudeDegrees);
    const double bLat = radians(b.latitudeDegrees);
    const double deltaLat = radians(b.latitudeDegrees - a.latitudeDegrees);
    const double deltaLon = radians(globe::shortest_longitude_delta(a.longitudeDegrees, b.longitudeDegrees));
    const double h = std::sin(deltaLat / 2.0) * std::sin(deltaLat / 2.0) + std::cos(aLat) * std::cos(bLat) * std::sin(deltaLon / 2.0) * std::sin(deltaLon / 2.0);
    return 2.0 * std::asin(std::sqrt(std::min(1.0, std::max(0.0, h))));
}
}

CountryMapStreaming::CountryMapStreaming(std::uint8_t maxResidentSlices)
    : maxResidentSlices_(std::max<std::uint8_t>(1U, maxResidentSlices)) {
    slices_.reserve(static_cast<std::size_t>(maxResidentSlices_) * 2U);
}

bool CountryMapStreaming::register_country(CountryAnchor anchor) {
    if (anchor.countryId.empty() || !globe::valid(anchor.center) || anchor.mapExtentKm <= 0.0F) return false;
    for (const auto& existing : countries_) if (existing.countryId == anchor.countryId) return false;
    countries_.push_back(std::move(anchor));
    return true;
}

std::string CountryMapStreaming::country_for_pin(globe::LatLon pin) const {
    if (!globe::valid(pin) || countries_.empty()) return {};
    const CountryAnchor* nearest = nullptr;
    double nearestDistance = std::numeric_limits<double>::max();
    for (const auto& country : countries_) {
        const auto distance = angular_distance(pin, country.center);
        if (distance < nearestDistance) { nearestDistance = distance; nearest = &country; }
    }
    return nearest == nullptr ? std::string{} : nearest->countryId;
}

bool CountryMapStreaming::request_from_globe_pin(globe::LatLon pin, std::uint8_t priority) {
    const auto countryId = country_for_pin(pin);
    if (countryId.empty()) return false;
    bool accepted = false;
    for (const auto kind : {SliceKind::CapitalCore, SliceKind::ResourceFrontier, SliceKind::WaterCorridor, SliceKind::RemoteSettlements}) {
        accepted = request_slice(countryId, kind, priority) || accepted;
    }
    return accepted;
}

bool CountryMapStreaming::request_slice(std::string countryId, SliceKind kind, std::uint8_t priority) {
    const auto countryExists = std::any_of(countries_.begin(), countries_.end(), [&](const CountryAnchor& anchor) { return anchor.countryId == countryId; });
    if (!countryExists) return false;
    for (auto& state : slices_) {
        if (state.request.countryId == countryId && state.request.kind == kind) {
            state.request.priority = std::max(state.request.priority, priority);
            state.requested = true;
            return true;
        }
    }
    slices_.push_back({{std::move(countryId), kind, priority}, false, true});
    return true;
}

void CountryMapStreaming::advance_frame(std::uint32_t frameBudget) {
    std::stable_sort(slices_.begin(), slices_.end(), [](const SliceState& left, const SliceState& right) {
        return left.request.priority > right.request.priority;
    });
    for (auto& state : slices_) {
        if (frameBudget == 0U || residentSlices_ >= maxResidentSlices_) break;
        if (state.requested && !state.loaded) {
            state.loaded = true;
            ++residentSlices_;
            --frameBudget;
        }
    }
}

void CountryMapStreaming::unload_country(std::string_view countryId) {
    for (auto& state : slices_) {
        if (state.request.countryId == countryId && state.loaded) {
            state.loaded = false;
            if (residentSlices_ > 0U) --residentSlices_;
        }
    }
}

std::vector<SliceState> CountryMapStreaming::states() const { return slices_; }
std::uint8_t CountryMapStreaming::max_resident_slices() const noexcept { return maxResidentSlices_; }
std::uint8_t CountryMapStreaming::resident_slice_count() const noexcept { return residentSlices_; }

} // namespace indoru::streaming
