# Indoru Engineering Architecture

## Core decision

Indoru का मुख्य game runtime **C++** में बनाया जाएगा। C++ engine integration, rendering, physics, world streaming, animation, input, gameplay simulation और performance-critical multiplayer client logic संभालेगा। C++ को modern standard, strict compiler warnings, automated tests और clear module boundaries के साथ उपयोग किया जाएगा। कोई language zero-bug guarantee नहीं देती; हमारा लक्ष्य bugs को design, testing, observability और staged releases से कम करना है।

## Language responsibilities

| Language | Primary responsibility | Rule |
|---|---|---|
| **C++** | Main game, renderer, physics, AI runtime, world streaming, save/load client and authoritative simulation modules | Core gameplay logic यहीं रहेगा |
| **Rust** | Optional safety-critical auxiliary services, protocol validators, asset/data validators और isolated high-reliability services | C++ में बार-बार FFI नहीं; stable API boundary पर उपयोग |
| **Go** | Optional lightweight infrastructure: matchmaking gateway, service discovery, telemetry collector, admin tools | केवल stateless/network-heavy services में |
| **TypeScript** | Map editor, content tools, web dashboards, launcher UI और data review tools | Game runtime का authoritative logic TypeScript में नहीं होगा |
| **JSON/Schema** | Country, city, village, flag, weather और mission content data | Generated/validated data; handwritten runtime constants कम रखें |

## C++ module layout

```text
engine/
  core/              lifecycle, logging, time, memory and error handling
  math/              vectors, transforms, geometry and deterministic helpers
  render/            materials, lighting, terrain and post-processing
  physics/           collision, vehicles, gravity and interactions
  world/             map streaming, regions, cities, villages and climate
  gameplay/          player, roles, missions, crime and justice systems
  ai/                citizens, schedules, perception and decision systems
  network/           replication, prediction, reconciliation and snapshots
  persistence/       save format, migrations and validation
  audio/              music, effects and spatial audio
  tools_bridge/      controlled interfaces to editor and external services
```

Each module owns its data and exposes a small interface. Rendering code cannot directly mutate justice state; AI cannot bypass server-authoritative validation; and UI tools cannot write arbitrary runtime memory. Shared types go into versioned schema packages rather than unowned utility headers.

## Multiplayer and load strategy

Server-authoritative systems will decide player position, inventory, economy, crime evidence, arrests, asset ownership and world events. The client predicts only short-lived movement and presentation. State will be divided into interest-management cells so a player receives high-detail updates near the active area rather than the entire world. City and village regions will stream as packages with explicit memory budgets.

Heavy operations such as pathfinding batches, NPC schedule evaluation, weather propagation and asset decompression will run in bounded worker pools. Frame-critical work stays on the main thread only when required. Queue sizes, time budgets and cancellation behavior will be explicit so load increases degrade gracefully instead of causing unbounded memory growth.

## Rust boundary

Rust is recommended for isolated tools and services where strong memory-safety guarantees provide clear benefit: binary asset validation, save-file validation, protocol fuzz targets and selected backend workers. C++ and Rust communicate through a small C ABI or versioned message protocol. Ownership, allocation rules, error codes and thread rules must be documented at the boundary. Rust should not be introduced merely to duplicate ordinary gameplay code.

## Go boundary

Go is optional for matchmaking, lobby coordination, telemetry ingestion, health checks and administrative services. These services should be stateless where possible and persist data through versioned APIs. Go services must not become a second source of truth for game simulation; the C++ authoritative simulation and persistence contracts remain canonical.

## TypeScript boundary

TypeScript is the preferred language for map/country editors, Coming Soon tables, flag metadata tools, content review dashboards and build-time validation UI. TypeScript tools read and write schema-validated JSON. They do not directly change compiled C++ behavior. A generated data manifest connects approved content to the C++ runtime.

## Bug-reduction policy

The project will use warnings-as-errors for production C++, static analysis, clang-format, clang-tidy, sanitizers in development builds, unit tests for math and rules, integration tests for save/load and network messages, deterministic simulation tests, replay tests and stress tests for world streaming. Every bug fix must include a regression test when practical.

Data migrations will be versioned. Save files will never depend on display names alone; stable IDs such as `country-indoru`, city IDs and village IDs are required. Country flags and settlement ownership are validated before runtime load. Unknown fields should be ignored only when safe; corrupted or incompatible data should produce a clear error and safe fallback.

## Legacy prevention

Every public module interface requires an owner, purpose, versioning note and test coverage expectation. New dependencies need a written reason, license review and removal plan. We will prefer a small number of stable libraries over a large stack of overlapping frameworks. Deprecated APIs receive a migration window and are removed only after usage reports are clean.

## Build profiles

`Debug` enables assertions and rich diagnostics. `Development` enables profiling, network simulation and selected sanitizers. `Release` enables optimization while retaining crash reporting and invariant checks that are safe in production. `Shipping` removes development-only tools but preserves telemetry, validation of untrusted network input and user-safe recovery behavior.

## Recommended first implementation order

First implement the C++ core loop, world coordinate system, deterministic math, data loader, Indoru map package, player movement and camera. Next add terrain streaming, one city district, one village, basic citizens and a small weather model. Only after the vertical slice is stable should we add Rust validators, Go matchmaking and TypeScript editor workflows. This order keeps the core game useful even when auxiliary services are offline.

## Non-negotiable rules

The main game code remains C++. No critical rule is duplicated independently in four languages. Every cross-language boundary has a versioned contract. Every network input is validated. Every save migration is tested. Every new city, village, country or flag is loaded from validated data rather than hardcoded ad hoc. Performance claims must be measured with profiling and load tests instead of assumed from language choice.
