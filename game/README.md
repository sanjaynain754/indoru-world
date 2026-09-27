# Indoru World — Navaar Playable Slice

This directory contains the first browser-playable vertical slice of **Indoru World**. It is a Babylon.js 3D district simulation set in fictional Navaar, with a drivable teal sedan, procedural roads and buildings, civilian traffic, pedestrian NPC routines, a riverfront mission, wanted pressure, health, minimap and local save/load.

## Run locally

```bash
pnpm install
pnpm dev
```

Open the Vite URL and use **WASD** or the arrow keys to drive; **Space** brakes. Touch devices show on-screen steering, brake and accelerator controls. The waypoint indicator shows the live direction and distance to the quay beacon. Press `M` for the world atlas, `E` to save, `L` to load and `R` to reset. Add `?demo` to the URL for a deterministic showcase route.

## Build

```bash
pnpm check
pnpm build
```

The GitHub Pages workflow builds `dist/public` and publishes the game automatically from the repository's `game/` directory.

## Scope boundary

This is a real playable foundation, not a claim that a full GTA-scale production is complete. Native C++ physics remains in `engine/cpp`; this browser slice provides the first visible game runtime and will be extended feature by feature with combat, police AI, mission chains, streaming and multiplayer services.
## Island and world navigation

Press **M** during play to open the original **Indoru World Atlas**. It connects playable Navaar/Avenra to the wider seven-reach island world and marks future regions as coming soon. The current rendering pass adds real-time shadows, warmer directional lighting, a more detailed sedan body and a cinematic atlas overlay; 4K output remains device/display dependent in a browser.
## World connectivity model

The atlas now represents the canonical seven regions: **Avarra Crescent**, **Khoruun Reach**, **Velmora Isles**, **Orsik Plateau**, **Nembasa Greenbelt**, **Dravik Arc** and **Erynd Polar Ring**. The world layer visualizes three original transport systems: bridge grid (gold), rail corridors (teal) and air routes (dashed white). These are the strategic world map foundation; physical crossings and airports will be streamed into each country slice as it becomes playable.
