# Indoru World — Navaar Playable Slice

This directory contains the first browser-playable vertical slice of **Indoru World**. It is a Babylon.js 3D district simulation set in fictional Navaar, with a drivable teal sedan, procedural roads and buildings, civilian traffic, pedestrian NPC routines, a riverfront mission, wanted pressure, health, minimap and local save/load.

## Run locally

```bash
pnpm install
pnpm dev
```

Open the Vite URL and use **WASD** or the arrow keys to drive. Press `E` to save, `L` to load and `R` to reset. Add `?demo` to the URL for a deterministic showcase route.

## Build

```bash
pnpm check
pnpm build
```

The GitHub Pages workflow builds `dist/public` and publishes the game automatically from the repository's `game/` directory.

## Scope boundary

This is a real playable foundation, not a claim that a full GTA-scale production is complete. Native C++ physics remains in `engine/cpp`; this browser slice provides the first visible game runtime and will be extended feature by feature with combat, police AI, mission chains, streaming and multiplayer services.
