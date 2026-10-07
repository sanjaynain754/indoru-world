# Indoru World Repository

Indoru एक original fictional global-world game concept है। यह repository उसके map structure, country names, flags, regions और future unlock data को canonical रूप से store करती है।

## Current state

The registry contains **120 fictional countries** across seven regions. All 120 countries are playable, with **Avenra** retained as the default starting country. Every country has a stable game-facing name, flag ID, map key and playable status.

Temporary labels such as `D1`, `D2`, `E1` and `E2` are not part of the canonical country data. Those labels may appear only as old visual-map references and must not be used by the game UI, server, save files or API.

## Repository layout

| Path | Purpose |
|---|---|
| `data/world.json` | Global registry, regions, default starting country and all-playable policy |
| `data/countries.json` | One canonical record for every country |
| `schemas/world.schema.json` | Validation rules for world and country data |
| `docs/world-structure.md` | Detailed map, naming, flags and expansion rules |
| `assets/maps/` | Global and local map artwork |
| `assets/flags/` | Future original SVG/PNG flag assets |
| `data/countries-source.md` | Repository-relative approved country design source |
| `scripts/build_world_data.py` | Rebuilds JSON from the approved country design source |
| `scripts/generate_country_templates.py` | Generates deterministic geography and reviewable content templates for all 120 countries |
| `scripts/generate_transport_network.py` | Generates road, bridge, rail/train-station and river transport contracts for all 120 countries |
| `scripts/generate_state_system.py` | Generates world → region → country → city/settlement state packages for all 120 countries |
| `scripts/generate_khoruun_baseline.py` | Generates Khoruun's 18-country, 72-map-slice production contract with weather and visual layers |
| `scripts/validate_world_data.py` | Checks cross-file identity, starter, duplicate-ID and asset invariants |
| `game/client/src/game/world/CountryTerrain.ts` | Loads generated heightfields into Babylon.js meshes when a country is selected |
| `game/client/src/game/world/CountryFeatureLayer.ts` | Renders generated rivers, lakes, mountains, roads, bridges, rail, settlements, airports, ports and bases |
| `engine/cpp/include/indoru/city_building_framework.hpp` | City zones, road segments, plots and infrastructure targets |
| `engine/cpp/include/indoru/residential_building_placement.hpp` | Residential building targets and road-aware placement cells |
| `engine/cpp/include/indoru/residential_simulation.hpp` | Household capacity, night distribution and daily schedule foundation |
| `tools/validate_ue5_world_manifest.py` | Validates the imported UE5 20-country vertical-slice manifest |
| `engine/cpp/include/indoru/world_simulation_bridge.hpp` | Connects transport links to city, residential and household simulation plans |
| `engine/cpp/include/indoru/state_system.hpp` | Validated hierarchy and overflow-safe population transitions |
| `engine/cpp/include/indoru/weather_simulation.hpp` | Deterministic weather sampling with traffic, NPC, rail, river and emergency effects |
| `docs/architecture-workflow-and-roadmap.md` | Native PC architecture diagram, data flow and implementation roadmap |

## UI behavior

The world-map screen should show every country using its official fictional name and flag preview. Every country should have an enabled action such as `Enter Country`; the `unlockOrder` field is a stable content-order hint, not a disabled-access gate.

Selecting `ENTER COUNTRY` for countries 002–120 fetches their published `world/terrain/<country>.json` package and renders its heightfield as a Babylon.js mesh. Avenra keeps the existing Navaar district scene as the starter fallback.

## Data rebuild

From the repository root, run:

```bash
python3 scripts/build_world_data.py
python3 scripts/generate_country_templates.py
python3 scripts/generate_transport_network.py
python3 scripts/generate_state_system.py
python3 scripts/generate_khoruun_baseline.py
python3 scripts/validate_world_data.py
```

The generator rebuilds `data/world.json` and `data/countries.json` from the approved source list. `generate_country_templates.py` creates one deterministic package for all 120 countries. Each package includes elevation, rivers, lakes, mountain landmarks, roads, bridges, villages, rail, optional airports, ports, military bases and route contracts for metro, airline, ship and submarine systems. These are playable procedural foundations marked `completion: template` and must be reviewed before production art/content replaces them. Use `--force` to regenerate existing packages or `--exclude-starter` to omit Avenra's generated package.

## Imported P1 simulation foundation

The uploaded P1 work has been integrated as a C++ simulation/data layer. It adds city-building targets, twelve city zones, road/plot validation, residential building placement targets, household capacity checks, night-time population distribution and schedules that cross midnight. These systems are deliberately engine-side foundations; they do not claim to be finished AAA building meshes, interiors, traffic AI or Unreal Engine assets.

The imported `data/ue5/20-country-world-partition-manifest.json` is a validated **20-country vertical-slice reference**, not a replacement for the canonical 120-country registry. Its validator is run with:

```bash
python3 tools/validate_ue5_world_manifest.py
```

## Design and copyright safeguards

All names, map geography, symbols and flag compositions are original fictional content. Do not import real national flags, government seals, corporate logos or recognizable protected emblems. Before any public commercial launch, run an independent trademark and legal review of the final names and visual assets.

## First playable target

The first detailed country package will be `country/avenra`, while the central island country **Indoru** remains the primary world concept and application identity. Its later local map package can be connected to this registry when the country-level game design is finalized.

## Playable game slice

The repository includes `game/`, a browser-playable Babylon.js debug/visualization slice of Navaar. It is not the final product target. The production direction is a native high-end PC runtime using the C++ simulation layer, streamed world data, PBR assets, transport graphs and a native renderer. See [`docs/architecture-workflow-and-roadmap.md`](docs/architecture-workflow-and-roadmap.md) for the complete architecture and staged implementation plan.
