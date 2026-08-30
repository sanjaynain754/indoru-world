# Indoru World Repository

Indoru एक original fictional global-world game concept है। यह repository उसके map structure, country names, flags, regions और future unlock data को canonical रूप से store करती है।

## Current state

The registry contains **120 fictional countries** across seven regions. **Avenra** is the only playable starter country in the first release. The other 119 countries are registered by their real game-facing fictional names and flag IDs, but their content status is `coming_soon`.

Temporary labels such as `D1`, `D2`, `E1` and `E2` are not part of the canonical country data. Those labels may appear only as old visual-map references and must not be used by the game UI, server, save files or API.

## Repository layout

| Path | Purpose |
|---|---|
| `data/world.json` | Global registry, regions, starting country and Coming Soon policy |
| `data/countries.json` | One canonical record for every country |
| `schemas/world.schema.json` | Validation rules for world and country data |
| `docs/world-structure.md` | Detailed map, naming, flags and expansion rules |
| `assets/maps/` | Global and local map artwork |
| `assets/flags/` | Future original SVG/PNG flag assets |
| `scripts/build_world_data.py` | Rebuilds JSON from the approved country design source |

## UI behavior

The world-map screen should show every country using its official fictional name and flag preview. A playable country should have an enabled action such as `Enter Country`. A registered but unfinished country should show its name, flag, region and short identity with the disabled label **Coming Soon**. The table may include planned unlock order, but it must never imply that unfinished terrain or missions are already available.

## Data rebuild

From the repository root, run:

```bash
python3 scripts/build_world_data.py
```

The script rebuilds `data/world.json` and `data/countries.json` from the approved source list. After changing a country name, flag ID or status, rebuild the files and review the diff before committing.

## Design and copyright safeguards

All names, map geography, symbols and flag compositions are original fictional content. Do not import real national flags, government seals, corporate logos or recognizable protected emblems. Before any public commercial launch, run an independent trademark and legal review of the final names and visual assets.

## First playable target

The first detailed country package will be `country/avenra`, while the central island country **Indoru** remains the primary world concept and application identity. Its later local map package can be connected to this registry when the country-level game design is finalized.
