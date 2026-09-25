# Game Plan: Indoru World — Navaar Vertical Slice

## Goal

Build a browser-playable first vertical slice of the fictional capital Navaar. The player drives a teal sedan through a compact riverfront district while traffic, pedestrians, mission state, wanted pressure, health and save/load systems run in one deterministic game loop.

## Risk Tasks

### 1. Third-person vehicle controller
- **Why isolated:** A chase camera and vehicle movement must feel stable while the player turns, accelerates and reverses in a browser render loop.
- **Approach:** Keep a simple arcade model in a plain TypeScript `Vehicle` class: forward velocity, drag, steering yaw, road-boundary clamp and camera follow. Use semantic keyboard actions through `InputManager`.
- **Verify:** Arrow/WASD input changes speed and heading; releasing input applies drag; the camera follows without clipping; `?demo` drives a repeatable route.

### 2. Procedural 3D district and traffic
- **Why isolated:** Many meshes and moving agents can create visual clutter or frame-rate regressions.
- **Approach:** Build a compact grid of instanced-style boxes for buildings, procedural roads and riverfront props. Keep traffic and pedestrian agents as lightweight Babylon meshes with bounded deterministic movement.
- **Verify:** City, roads, river, elevated rail and skyline are visible; civilian cars and NPCs move; no mesh escapes the district bounds.

### 3. HUD state and persistence
- **Why isolated:** Health, mission progress, wanted level, minimap, speed and save/load must remain synchronized with the scene loop.
- **Approach:** Use a DOM HUD controller subscribed to a plain `GameWorld` snapshot. Store a compact save record in `localStorage`; validate and fall back safely when absent or malformed.
- **Verify:** HUD updates while driving, mission progress changes at checkpoints, wanted stars change after collisions, save/load restores position and health, and minimap tracks the player.

## Main Build

- **Assets:** generated Navaar visual target and skyline backdrop in managed storage; procedural car, buildings, roads, traffic cars, NPCs and props.
- **Core gameplay:** drive the sedan, reach the riverfront checkpoint mission, avoid traffic, trigger a mild wanted response on collisions, and keep health visible.
- **Controls:** WASD or arrow keys to drive; `E` to save; `L` to load; `R` to reset; `?demo` runs an autopilot route for visual verification.
- **Verify:**
  - Movement direction matches player input and camera follows the car.
  - Traffic and pedestrians move without leaving the district.
  - Mission card, health bar, speed readout, wanted stars and minimap remain readable.
  - Generated skyline asset is used as a visual background layer.
  - No browser console errors, TypeScript errors or missing runtime assets.
  - Build and preview complete successfully.
