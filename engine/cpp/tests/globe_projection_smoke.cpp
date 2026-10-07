#include "indoru/globe_projection.hpp"
#include <cassert>
#include <cmath>

using namespace indoru::globe;

int main() {
    assert(valid({90.0, 180.0}));
    assert(!valid({91.0, 0.0}));
    const auto pole = to_unit_sphere({90.0, 0.0});
    assert(std::abs(pole.y - 1.0) < 1e-9);

    const LatLon datelineStart{0.0, 179.0};
    const LatLon datelineEnd{0.0, -179.0};
    assert(std::abs(shortest_longitude_delta(datelineStart.longitudeDegrees, datelineEnd.longitudeDegrees) - 2.0) < 1e-9);
    const auto halfway = interpolate_shortest(datelineStart, datelineEnd, 0.5);
    assert(std::abs(std::abs(halfway.longitudeDegrees) - 180.0) < 1e-9);

    const auto original = LatLon{42.5, -71.25};
    const auto roundTrip = from_unit_sphere(to_unit_sphere(original));
    assert(std::abs(roundTrip.latitudeDegrees - original.latitudeDegrees) < 1e-9);
    assert(std::abs(roundTrip.longitudeDegrees - original.longitudeDegrees) < 1e-9);

    PinAnimation animation{{20.0, 10.0}, {65.0, 100.0}, 1.0, 0.0};
    const auto start = sample(animation, 0.0);
    assert(std::abs(start.latitudeDegrees - 20.0) < 1e-9);
    const auto end = sample(animation, 1.0);
    assert(std::abs(end.latitudeDegrees - 65.0) < 1e-9);
    assert(std::abs(end.longitudeDegrees - 100.0) < 1e-9);
    return 0;
}
