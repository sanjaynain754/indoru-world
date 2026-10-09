# Earth Globe and Gravity Branch

This branch owns planet-wide systems and is intentionally separate from region branches such as `region/khoruun-reach`.

## Coordinate model

The globe uses geodetic latitude and longitude in degrees, with north and east positive. Rendering may use a sphere for stable visual presentation, while gameplay coordinates retain latitude/longitude and altitude. Country pins use shortest wrapped longitude rotation so a dateline crossing never causes an unnecessary long spin.

The reference contract follows WGS84-style geodesy: an ellipsoid reference frame, geodetic latitude, longitude and ellipsoid height. The current runtime gravity implementation uses WGS84 Somigliana normal gravity on the reference ellipsoid plus a bounded altitude correction. It is not a full EGM geoid or local anomaly model; those can be added later if simulation requires them.

## Gravity behavior

At sea-level reference height, gravity varies with latitude: it is lower near the equator and higher near the poles. Altitude reduces normal gravity. The API returns both acceleration in m/s² and the unit direction toward the globe center, allowing physics systems to apply gravity relative to a curved world rather than a fixed global `-Y` vector.

Input safety bounds are latitude `[-90°, +90°]` and gameplay altitude `[-1,000 m, +100,000 m]`. Invalid input returns a safe zero sample rather than propagating NaN values into the physics engine.

## Day and night

The planet clock models a solar day, an orbital year and axial tilt. The runtime derives solar declination, the subsolar longitude, local solar hour, solar elevation and azimuth. It exposes both hard daylight and civil twilight, plus a bounded daylight factor for lighting and sky blending. Polar regions therefore receive long seasonal day/night periods rather than an artificial fixed 12-hour cycle.

The implementation is deliberately deterministic and presentation-friendly: terrain, weather and NPC systems can sample the same clock and location to remain synchronized. The renderer-side sun, sky and shadow integration now exists as `indoru::sky` and consumes this state rather than inventing a second time model.

## Ocean, coast and beach foundation

`indoru::ocean` is the native planetary water contract. It consumes the same geodetic location and gravity sample as the globe system and provides:

- sea level plus deterministic M2-like tide elevation;
- deep-ocean, shallow-sea, beach, tidal-flat and rocky-coast classification;
- fetch-limited wind waves combined with swell;
- shallow-water wave breaking, foam factor, wavelength and crest speed;
- deterministic local surface elevation and horizontal wave velocity;
- gravity-aligned buoyancy acceleration for boats, swimmers and floating debris.

This is a simulation foundation rather than a renderer. A future renderer can use `surfaceElevationMeters`, `surfaceVelocityXMetersPerSecond`, `surfaceVelocityZMetersPerSecond` and `foamFactor` for animated water, shore foam, spray and reflections without creating a second water clock. Terrain or bathymetry streaming supplies `CoastSample::terrainElevationMeters` and `seabedDepthMeters`; the native system then remains valid for the whole globe, including beaches and storm coasts.

## Game atmosphere and synthetic ocean fields

`indoru::atmosphere` provides a deliberately stylized, seed-driven atmosphere rather than a copied real-world weather dataset. It samples temperature, pressure, humidity, cloud factor, precipitation, air density and an east/north/up wind vector from location, simulation time, altitude and a region-friendly forcing profile. Wind direction is expressed as degrees clockwise from north. The field is deterministic, bounded and suitable for gameplay, streaming and replay.

The ocean system now derives synthetic water temperature, salinity, density and east/north current velocity from the same location, season and seed. These fields are not intended to reproduce Earth’s actual coastlines or currents. They create consistent game behavior for boats, swimming, fishing, storms and renderer effects while allowing region branches to override local parameters.

## Mathematical global terrain tiles

`indoru::terrain` provides a procedural equirectangular tile contract. A tile is addressed by `{level, x, y}` over the complete longitude/latitude domain, with deterministic bounds and grid generation. Its mathematical field produces land elevation, ocean bathymetry depth, slope and moisture from a seed. This gives the renderer and region streaming systems a stable global scaffold without importing real-world geography. Region-owned terrain packages can later replace or blend the field at selected tiles while preserving the same tile identity and sampling API.

## Sky, sun, moon and shadow integration

`indoru::sky` is the renderer-facing lighting contract. It never re-derives solar geometry: it consumes a `planet::SunState` plus an optional local `atmosphere::State` and produces the presentation values a renderer needs.

- Sun: unit direction in the globe frame (east/north/up basis at the sampled location), irradiance, and an air-mass-driven colour that reddens toward the horizon. Civil twilight contributes a bounded ambient term so the transition at the horizon stays continuous.
- Sky and ambient: zenith, horizon and ambient linear-RGB colours blended from the day-night model's own `daylightFactor`, with a warm sunset band that peaks at the horizon and is damped by cloud cover.
- Fog: colour derived from the sky gradient plus a density that grows with cloud cover and humidity and decays with observer altitude.
- Stars: visibility suppressed by daylight and cloud, with a deterministic seed-driven night-sky tint.
- Moon: a phase-shifted view of the same clock. The synodic month drives the elongation from the sun, so new moon sits with the sun and full moon sits opposite it; illumination, direction, elevation and intensity follow from that single phase.
- Shadow: light direction, a ground-projected length factor that is disabled at or below the horizon, softness from sun altitude and cloud cover, and intensity for shadow-map blending.
- Exposure: a bounded multiplier hint (1.0 in full daylight, larger at night) so tone mapping does not need a second brightness model.

All colours are bounded to the 0..1 linear range and invalid input returns a safe default state instead of NaN values.

## Unified environment sample

`indoru::environment` is the single entry point for the runtime and streaming layers. One call at one location and one simulation time returns gravity, ground, atmosphere, sun, sky and water derived from the same clock and the same forcing, so no subsystem invents its own time or forcing model.

It wires the subsystems together rather than duplicating them: terrain supplies elevation, bathymetry, slope and the ocean flag; gravity is sampled at the terrain surface through the documented altitude clamp; the orbital phase feeds the atmosphere; the atmosphere's wind drives the ocean forcing; and the sky consumes the resulting sun and air state. `State::valid` reports whether the inputs were accepted, so a rejected sample is distinguishable from a legitimate all-zero one.

Region branches override the per-system `Config` values instead of forking the pipeline, which keeps Khoruun-style regional work additive.

## Branch ownership

- `world/globe-gravity`: planet coordinates, globe projection, Earth gravity, global climate bands and future orbital/rotation systems.
- `region/khoruun-reach`: Khoruun terrain, weather partitions, snow/dust presentation and four-slice country streaming.
- `main`: integration baseline; future region work must arrive through pull requests.

## References

- [NOAA — Datums and Reference Frames](https://geodesy.noaa.gov/datums/index.shtml)
- [MathWorks — gravitywgs84](https://www.mathworks.com/help/aerotbx/ug/gravitywgs84.html)
