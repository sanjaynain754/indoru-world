# Earth Globe and Gravity Branch

This branch owns planet-wide systems and is intentionally separate from region branches such as `region/khoruun-reach`.

## Coordinate model

The globe uses geodetic latitude and longitude in degrees, with north and east positive. Rendering may use a sphere for stable visual presentation, while gameplay coordinates retain latitude/longitude and altitude. Country pins use shortest wrapped longitude rotation so a dateline crossing never causes an unnecessary long spin.

The reference contract follows WGS84-style geodesy: an ellipsoid reference frame, geodetic latitude, longitude and ellipsoid height. The current runtime gravity implementation uses WGS84 Somigliana normal gravity on the reference ellipsoid plus a bounded altitude correction. It is not a full EGM geoid or local anomaly model; those can be added later if simulation requires them.

## Gravity behavior

At sea-level reference height, gravity varies with latitude: it is lower near the equator and higher near the poles. Altitude reduces normal gravity. The API returns both acceleration in m/s² and the unit direction toward the globe center, allowing physics systems to apply gravity relative to a curved world rather than a fixed global `-Y` vector.

Input safety bounds are latitude `[-90°, +90°]` and gameplay altitude `[-1,000 m, +100,000 m]`. Invalid input returns a safe zero sample rather than propagating NaN values into the physics engine.

## Branch ownership

- `world/globe-gravity`: planet coordinates, globe projection, Earth gravity, global climate bands and future orbital/rotation systems.
- `region/khoruun-reach`: Khoruun terrain, weather partitions, snow/dust presentation and four-slice country streaming.
- `main`: integration baseline; future region work must arrive through pull requests.

## References

- [NOAA — Datums and Reference Frames](https://geodesy.noaa.gov/datums/index.shtml)
- [MathWorks — gravitywgs84](https://www.mathworks.com/help/aerotbx/ug/gravitywgs84.html)
