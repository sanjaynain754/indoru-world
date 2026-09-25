# Indoru World — Navaar Runtime Structure

## Host

React is only the full-screen canvas host and HUD shell. Babylon owns the 3D scene, render loop and meshes. Gameplay modules are framework-agnostic TypeScript classes under `client/src/game/`.

## Modules

- `client/src/components/GameCanvas.tsx` — lifecycle-safe Babylon engine host.
- `client/src/game/scene.ts` — creates scene, lights, camera, district geometry, world and HUD.
- `client/src/game/world/GameWorld.ts` — owns player vehicle, traffic, NPCs, mission, wanted, health, save/load and update order.
- `client/src/game/input/InputManager.ts` — semantic keyboard state and cleanup.
- `client/src/game/actors/Vehicle.ts` — arcade driving state and Babylon mesh.
- `client/src/game/actors/Traffic.ts` — bounded civilian vehicle agents.
- `client/src/game/actors/Npc.ts` — pedestrian routine agents.
- `client/src/game/ui/HudController.ts` — DOM HUD rendering and minimap projection.

## State contract

`GameWorld.snapshot()` returns a serializable snapshot with player position, speed, health, wanted level, mission progress and agent counts. HUD only consumes this snapshot. Save data is versioned with `schemaVersion: 1`.

## Scene contract

`createGameScene(engine, canvas)` returns `{ scene, dispose }`. Scene disposal removes the update observer, input listeners and all meshes/materials through Babylon scene disposal.

## Asset hints

- Managed skyline: `/manus-storage/navaar-skyline_eadadc5f.png` as a distant billboard/background plane.
- Procedural geometry is used for the playable district to keep the first slice fast and deterministic.
- Palette: indigo night, warm amber windows, turquoise road accents, sandstone city massing.
