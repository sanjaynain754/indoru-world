# Khoruun Reach — Production Map and Weather Specification

## Source-of-truth rule

The uploaded ZIP contains no map images or weather assets. The supplied master-map image is treated as a **conceptual Avarra reference only**. It is not imported as canonical geography, and generated labels are not accepted as game locations. Khoruun uses the approved `data/world.json`, country records, settlement records and generated terrain packages.

## Map organization

Khoruun contains **18 canonical country maps**. Each country is split into four reviewable map slices, for **72 map slices** total:

1. **Capital core** — capital marker, civic district, city services, roads and rail.
2. **Resource frontier** — mountains, quarry/mining/solar/industrial edge, remote roads and military/emergency sites.
3. **Water and corridor** — rivers/lakes/coast, bridges, ports, waterways and transport corridors where geography supports them.
4. **Remote settlements** — villages, wilderness, emergency depots, all-weather routes and hazard boundaries.

Every slice exposes the same map layers:

- Country border
- Capital marker
- City and village markers
- Highways and normal roads
- Railway and stations
- Rivers, lakes and waterways
- Bridges and tunnels
- Mountains and terrain
- Weather overlay
- Compass and scale bar

A map slice is a streaming/content boundary, not permission to invent extra canonical place names. Names must resolve to the country and settlement registry.

## Weather contract

Weather is simulation data, not only a visual effect. Each Khoruun country receives:

- Climate family and season sequence
- Hazard list
- Road-grip range
- Visibility range
- NPC activity range
- 24-hour forecast cycle with 15-minute simulation updates
- Deterministic seed inputs: `countryId`, terrain seed and simulation day
- Responses such as canyon-road closure, flood warning, freight hold, snow clearance, warm-shelter activation and emergency rerouting

Weather affects:

- Vehicle traffic and braking/grip
- Visibility and navigation
- Rail delays and maintenance windows
- River level and water-route availability
- NPC schedules and outdoor activity
- Emergency calls and rescue dispatch
- Power demand and asset wear

## Spherical world and pin behavior

Khoruun is authored on a globe coordinate contract rather than a permanently flat map. Every country and settlement pin carries latitude and longitude, which the native runtime converts to a unit-sphere position. When a user selects any pin, the globe controller must use the shortest wrapped longitude path, so a pin near +179° to a pin near -179° rotates across the dateline instead of spinning almost a full revolution. The default transition uses a smoothstep curve and an 0.8-second duration, with the duration adjustable by the native presentation layer.

The region is divided into three visual climate bands. The northern highlands are snow and ice with blizzard and avalanche risk. The middle belt combines cold plateau, forest and rockfall exposure. The southern belt contains arid canyons, salt flats and flash-flood or heatwave risk. These bands are presentation and simulation hints; final weather remains driven by the deterministic native weather sampler and local terrain/elevation data.

The band is now derived from each country's globe latitude rather than selected only from its fictional identity. Khoruun country placement spans approximately 42°N to 71°N: below 52°N uses the South Red Canyon partition, 52°N–63.999°N uses Middle Polar, and 64°N or higher uses North Polar. Altitude, terrain and coast exposure refine the local weather after this global classification. This keeps the upper side of Khoruun cold while allowing its southern belt to remain a believable red-canyon and salt-flat transition.

## Production acceptance gates

A Khoruun country is not complete until:

- All four map slices load and stream without missing references.
- Borders, capital, city and village markers resolve to canonical IDs.
- Roads, rail, bridges, rivers and ports link to generated transport contracts.
- Weather changes at runtime and affects at least one traffic, NPC and emergency loop.
- Concept images are either replaced by approved assets or remain explicitly reference-only.
- No generated image label is promoted to a canonical location.
- Native C++ state, transport and terrain validators pass.
- Desktop high-end performance and streaming budgets are measured.

## Generator

```bash
python3 scripts/generate_khoruun_baseline.py
```

Output:

```text
countries=18
maps=72
weatherProfiles=18
```

The output is `data/region-baselines/khoruun-reach.json`. It is a production planning contract; final terrain meshes, textures, lighting, VFX and authored city assets remain native-runtime work.

The native implementation is in `engine/cpp/include/indoru/globe_projection.hpp` and `engine/cpp/src/globe_projection.cpp`. Its regression test covers pole conversion, coordinate round-trip, dateline wrapping and pin animation completion.
