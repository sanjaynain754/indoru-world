#!/usr/bin/env python3
"""Validate cross-file invariants that JSON Schema cannot express."""

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def fail(message: str) -> None:
    print(f"ERROR: {message}", file=sys.stderr)
    raise SystemExit(1)


world = json.loads((ROOT / "data/world.json").read_text(encoding="utf-8"))
countries = json.loads((ROOT / "data/countries.json").read_text(encoding="utf-8"))
if world.get("countries") != countries:
    fail("data/world.json countries differs from data/countries.json")
if world.get("name") != "Indoru" or world.get("worldId") != "indoru-world-001":
    fail("world identity is not canonical Indoru")
if world.get("countryCount") != len(countries):
    fail("countryCount does not match countries.json")
policy = world.get("comingSoonPolicy", {})
if policy.get("playable") is not True or policy.get("unlockByExpansion") is not False or policy.get("label") != "Available Now":
    fail("availability policy must mark every country Available Now")

ids = [country.get("countryId") for country in countries]
if len(ids) != len(set(ids)):
    fail("country IDs are not unique")
if any(country.get("status") != "playable" for country in countries):
    fail("every country must be playable")
if any(country.get("unlockOrder") != index for index, country in enumerate(countries, 1)):
    fail("playable countries must have sequential unlockOrder values")
starter = countries[0]
if starter.get("name") != "Avenra" or world.get("startingCountryId") != starter.get("countryId"):
    fail("Avenra must remain the default starting country")

asset_refs = [world.get("mapAsset")] + [country.get("flagAsset") for country in countries]
missing = [ref for ref in asset_refs if not ref or not (ROOT / ref).is_file()]
if missing:
    fail(f"missing asset references: {', '.join(missing[:5])}")

content_root = ROOT / "data" / "country-content"
transport_root = ROOT / "data" / "transport-network"
state_root = ROOT / "data" / "state-system"
for country in countries:
    package_dir = content_root / country["name"].lower()
    terrain_path = package_dir / "terrain.json"
    template_path = package_dir / "content-template.json"
    frontend_terrain_path = ROOT / "game" / "client" / "public" / "world" / "terrain" / f"{country['name'].lower()}.json"
    if not terrain_path.is_file() or not template_path.is_file() or not frontend_terrain_path.is_file():
        fail(f"missing generated package for {country['countryId']}")
    terrain = json.loads(terrain_path.read_text(encoding="utf-8"))
    template = json.loads(template_path.read_text(encoding="utf-8"))
    grid = terrain.get("grid", {})
    if terrain.get("countryId") != country["countryId"] or len(grid.get("heightsM", [])) != grid.get("columns", 0) * grid.get("rows", 0):
        fail(f"invalid terrain package for {country['countryId']}")
    if template.get("countryId") != country["countryId"] or template.get("status") != "playable" or template.get("completion") != "template":
        fail(f"invalid content template for {country['countryId']}")
    features = terrain.get("features", {})
    required_features = ("rivers", "lakes", "mountains", "roads", "bridges", "settlements", "railways", "airports", "ports", "militaryBases", "metroLines", "airRoutes", "shipLanes", "submarineZones")
    if any(name not in features for name in required_features):
        fail(f"incomplete feature package for {country['countryId']}")
    if len(features["settlements"]) < 2 or len(features["roads"]) < 1 or len(features["rivers"]) < 1:
        fail(f"country {country['countryId']} lacks basic geography features")
    if country.get("region") == "Khoruun Reach":
        winter_path = package_dir / "winter-life.json"
        if not winter_path.is_file():
            fail(f"missing Khoruun winter-life package for {country['countryId']}")
        winter = json.loads(winter_path.read_text(encoding="utf-8"))
        if winter.get("countryId") != country["countryId"] or winter.get("regionId") != "region-khoruun-reach":
            fail(f"winter-life identity mismatch for {country['countryId']}")
        if len(winter.get("residentialDistricts", [])) < 3 or len(winter.get("shopsAndServices", [])) < 4:
            fail(f"winter-life package incomplete for {country['countryId']}")
        showroom = winter.get("showroom", {})
        if not showroom.get("exhibits") or not showroom.get("interactiveActions"):
            fail(f"winter showroom incomplete for {country['countryId']}")
    transport_path = transport_root / f"{country['name'].lower()}.json"
    if not transport_path.is_file():
        fail(f"missing transport network for {country['countryId']}")
    transport = json.loads(transport_path.read_text(encoding="utf-8"))
    if transport.get("countryId") != country["countryId"]:
        fail(f"transport identity mismatch for {country['countryId']}")
    if not transport.get("roadNetwork", {}).get("segments") or not transport.get("railNetwork", {}).get("lines"):
        fail(f"transport network incomplete for {country['countryId']}")

transport_manifest = transport_root / "manifest.json"
if not transport_manifest.is_file():
    fail("transport network manifest is missing")
manifest = json.loads(transport_manifest.read_text(encoding="utf-8"))
if manifest.get("countryCount") != len(countries) or len(manifest.get("packages", [])) != len(countries):
    fail("transport manifest does not cover all countries")

state_manifest_path = state_root / "manifest.json"
if not state_manifest_path.is_file():
    fail("state-system manifest is missing")
state_manifest = json.loads(state_manifest_path.read_text(encoding="utf-8"))
if state_manifest.get("countryCount") != len(countries) or state_manifest.get("worldId") != world.get("worldId"):
    fail("state-system manifest does not cover the canonical world")
for country in countries:
    state_path = state_root / f"{country['name'].lower()}.json"
    if not state_path.is_file():
        fail(f"missing state package for {country['countryId']}")
    state_package = json.loads(state_path.read_text(encoding="utf-8"))
    if state_package.get("countryId") != country["countryId"] or not state_package.get("stateIds"):
        fail(f"invalid state package for {country['countryId']}")

print(f"World data valid: {len(countries)} playable countries, default={starter['name']}, assets={len(asset_refs)}")
