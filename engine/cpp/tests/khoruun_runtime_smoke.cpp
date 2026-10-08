#include "indoru/country_map_streaming.hpp"
#include "indoru/weather_partition.hpp"
#include <cassert>

int main() {
    using namespace indoru;
    const auto north = weather::profile_for(weather::Band::NorthPolar);
    const auto middle = weather::profile_for(weather::Band::MiddlePolar);
    const auto south = weather::profile_for(weather::Band::SouthRedCanyon);
    assert(weather::validate_profile(north));
    assert(north.snowRate > middle.snowRate);
    assert(south.dustRate > middle.dustRate);

    weather::SnowDustShowroom showroom(64);
    showroom.set_partition(south);
    showroom.advance(0.1F, 1U);
    assert(showroom.active_particle_count() > 0U);
    assert(showroom.active_particle_count() <= 64U);
    showroom.set_partition(north);
    showroom.advance(0.1F, 2U);
    assert(showroom.active_particle_count() <= 64U);

    streaming::CountryMapStreaming streaming(4);
    assert(streaming.register_country({"country-021", {55.0, 30.0}, 100.0F}));
    assert(streaming.register_country({"country-022", {40.0, 75.0}, 100.0F}));
    assert(!streaming.register_country({"country-021", {55.0, 30.0}, 100.0F}));
    assert(streaming.country_for_pin({54.9, 30.1}) == "country-021");
    assert(streaming.request_from_globe_pin({54.9, 30.1}, 255));
    streaming.advance_frame(4U);
    assert(streaming.resident_slice_count() == 4U);
    assert(streaming.states().size() == 4U);
    streaming.unload_country("country-021");
    assert(streaming.resident_slice_count() == 0U);
    return 0;
}
