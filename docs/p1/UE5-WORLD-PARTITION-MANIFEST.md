# Indoru World — UE5 World Construction Manifest

## Purpose
This is the first implementation step after the 20-country world-data package.

It converts each country's world definition into a UE5-oriented production manifest for:
- World Partition
- Runtime Data Layers
- HLOD
- World Partition navigation
- Mass-style crowd simulation
- Mass-style traffic simulation
- building/service placement
- country/Indoru flag rules

## Per-country structure

Every playable country receives:

**1 capital + 3 major cities + 3 villages**

Each location has:
- unique location ID
- density/streaming profile
- Data Layers
- required service zones
- flag protocol
- weather binding
- NPC/traffic/crowd streaming hooks

## Flag rule

Country identity is never removed.

- Country-owned facility: **country flag**
- Shared Indoru facility: **Indoru flag**
- Joint facility: **country flag + Indoru flag**

## Population rule

The 20-country simulation totals exactly **10,000,000 residents**.

That population is simulated across fidelity tiers. It is not 10 million heavyweight Unreal Actors loaded simultaneously.

## What this file does NOT claim

It does not mean final AAA 3D buildings, interiors, animations, VFX, audio or finished terrain are already authored.

The next stage is to create the UE5 DataAssets/Actor classes/importer that consume this manifest.

## Validation

Run:

`python tools/validate_ue5_world_manifest.py`

Expected result:

`UE5_WORLD_MANIFEST_PASS`
