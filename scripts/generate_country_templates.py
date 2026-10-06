#!/usr/bin/env python3
"""Generate deterministic starter terrain and content templates for every non-starter country.

The output is intentionally data-only: it gives the runtime a valid heightfield and a
complete content contract without pretending that placeholder missions or settlements
are finished production content.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import math
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = ROOT / "data" / "country-content"
DEFAULT_FRONTEND_OUTPUT = ROOT / "game" / "client" / "public" / "world" / "terrain"
GRID_SIZE = 17
WORLD_HALF_EXTENT_M = 128.0

REGION_BIOMES = {
    "Avarra Crescent": ("temperate-river", 0.18, 0.42),
    "Khoruun Reach": ("canyon-steppe", 0.32, 0.22),
    "Velmora Isles": ("coastal-island", 0.12, 0.65),
    "Orsik Plateau": ("highland-basin", 0.46, 0.28),
    "Nembasa Greenbelt": ("rainforest-wetland", 0.10, 0.78),
    "Dravik Arc": ("volcanic-ashland", 0.38, 0.52),
    "Erynd Polar Ring": ("polar-tundra", 0.24, 0.16),
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate country terrain and content templates.")
    parser.add_argument("--world", type=Path, default=ROOT / "data" / "world.json")
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--frontend-output", type=Path, default=DEFAULT_FRONTEND_OUTPUT,
                        help="frontend-public directory for runtime terrain JSON")
    parser.add_argument("--include-starter", action="store_true", default=True, help="kept for backwards compatibility; included by default")
    parser.add_argument("--exclude-starter", dest="include_starter", action="store_false", help="exclude the handcrafted Avenra starter package")
    parser.add_argument("--force", action="store_true", help="replace existing generated country packages")
    return parser.parse_args()


def stable_seed(country_id: str) -> int:
    return int.from_bytes(hashlib.sha256(country_id.encode("utf-8")).digest()[:8], "big")


def random_unit(seed: int, salt: str) -> float:
    digest = hashlib.sha256(f"{seed}:{salt}".encode("utf-8")).digest()
    return int.from_bytes(digest[:8], "big") / float(2**64 - 1)


def feature_point(seed: int, salt: str, index: int, margin: float = 0.08) -> tuple[float, float]:
    x = (random_unit(seed, f"{salt}:x:{index}") * (1.0 - margin * 2.0) + margin) * 2.0 - 1.0
    z = (random_unit(seed, f"{salt}:z:{index}") * (1.0 - margin * 2.0) + margin) * 2.0 - 1.0
    return round(x * WORLD_HALF_EXTENT_M, 2), round(z * WORLD_HALF_EXTENT_M, 2)


def terrain_height(terrain: dict[str, Any], x: float, z: float) -> float:
    grid = terrain["grid"]
    column = max(0, min(grid["columns"] - 1, round((x - grid["originXM"]) / grid["cellSizeM"])))
    row = max(0, min(grid["rows"] - 1, round((z - grid["originZM"]) / grid["cellSizeM"])))
    return round(grid["heightsM"][row * grid["columns"] + column], 2)


def polyline(seed: int, salt: str, start: tuple[float, float], end: tuple[float, float], points: int = 6) -> list[dict[str, float]]:
    result = []
    for index in range(points):
        t = index / (points - 1)
        bend = (random_unit(seed, f"{salt}:bend:{index}") - 0.5) * 24.0 if index not in (0, points - 1) else 0.0
        x = start[0] * (1.0 - t) + end[0] * t + bend
        z = start[1] * (1.0 - t) + end[1] * t - bend * 0.55
        result.append({"x": round(x, 2), "y": round(terrain_height(_TERRAIN_CONTEXT, x, z), 2), "z": round(z, 2)})
    return result


_TERRAIN_CONTEXT: dict[str, Any] = {}


def features_for(country: dict[str, Any], terrain: dict[str, Any]) -> dict[str, Any]:
    global _TERRAIN_CONTEXT
    _TERRAIN_CONTEXT = terrain
    seed = terrain["seed"]
    water_level = terrain["surface"]["waterLevelM"]
    river_count = 1 + int(random_unit(seed, "river-count") * 2.5)
    rivers = []
    for index in range(river_count):
        side = -1.0 if index % 2 == 0 else 1.0
        start = (-WORLD_HALF_EXTENT_M, (random_unit(seed, f"river:{index}:start") * 2 - 1) * WORLD_HALF_EXTENT_M) if side < 0 else ((random_unit(seed, f"river:{index}:start") * 2 - 1) * WORLD_HALF_EXTENT_M, -WORLD_HALF_EXTENT_M)
        end = (WORLD_HALF_EXTENT_M, (random_unit(seed, f"river:{index}:end") * 2 - 1) * WORLD_HALF_EXTENT_M) if side < 0 else ((random_unit(seed, f"river:{index}:end") * 2 - 1) * WORLD_HALF_EXTENT_M, WORLD_HALF_EXTENT_M)
        rivers.append({"id": f"{country['countryId']}-river-{index + 1}", "name": f"{country['name']} River {index + 1}", "widthM": round(5 + random_unit(seed, f"river:{index}:width") * 16, 1), "points": polyline(seed, f"river:{index}", start, end, 7)})
    lake_count = 1 + int(random_unit(seed, "lake-count") * 2.2)
    lakes = []
    for index in range(lake_count):
        x, z = feature_point(seed, "lake", index, 0.2)
        lakes.append({"id": f"{country['countryId']}-lake-{index + 1}", "name": f"{country['name']} Lake {index + 1}", "x": x, "y": round(water_level + 0.01, 2), "z": z, "radiusM": round(8 + random_unit(seed, f"lake:{index}:radius") * 18, 1), "type": "reservoir" if index == 0 and random_unit(seed, "reservoir") > 0.55 else "natural"})
    settlement_count = 3 + int(random_unit(seed, "settlement-count") * 5)
    settlements = [{"id": f"{country['countryId']}-capital", "name": f"{country['name']} Central", "type": "capital", "populationTier": "large", "x": 0.0, "y": round(terrain_height(terrain, 0, 0) + 0.5, 2), "z": 0.0}]
    for index in range(settlement_count - 1):
        x, z = feature_point(seed, "settlement", index)
        settlements.append({"id": f"{country['countryId']}-village-{index + 1}", "name": f"{country['name']} Village {index + 1}", "type": "village", "populationTier": "small" if index % 3 else "medium", "x": x, "y": round(terrain_height(terrain, x, z) + 0.5, 2), "z": z})
    roads = []
    for index, settlement in enumerate(settlements[1:]):
        start = (settlements[0]["x"], settlements[0]["z"])
        end = (settlement["x"], settlement["z"])
        roads.append({"id": f"{country['countryId']}-road-{index + 1}", "class": "highway" if index == 0 else "regional", "points": polyline(seed, f"road:{index}", start, end, 5)})
    airports = []
    if random_unit(seed, "airport") > 0.12:
        x, z = feature_point(seed, "airport", 0, 0.25)
        airports.append({"id": f"{country['countryId']}-airport-1", "name": f"{country['name']} International", "size": "large" if random_unit(seed, "airport-size") > 0.6 else "regional", "x": x, "y": round(terrain_height(terrain, x, z) + 0.2, 2), "z": z, "runwayLengthM": 1800 if random_unit(seed, "runway") > 0.55 else 900})
    military_bases = []
    if random_unit(seed, "military-base") > 0.42:
        x, z = feature_point(seed, "military", 0, 0.18)
        military_bases.append({"id": f"{country['countryId']}-base-1", "name": f"{country['name']} Defense Station", "type": "air-ground" if airports else "ground", "x": x, "y": round(terrain_height(terrain, x, z) + 0.3, 2), "z": z})
    bridges = []
    for index, river in enumerate(rivers):
        midpoint = river["points"][len(river["points"]) // 2]
        bridges.append({"id": f"{country['countryId']}-bridge-{index + 1}", "name": f"{country['name']} Crossing {index + 1}", "roadId": roads[index % len(roads)]["id"], "riverId": river["id"], "x": midpoint["x"], "y": midpoint["y"] + 0.35, "z": midpoint["z"], "lengthM": round(river["widthM"] + 12, 1)})
    railways = [{"id": f"{country['countryId']}-rail-1", "class": "intercity", "points": polyline(seed, "rail", (settlements[0]["x"], settlements[0]["z"]), (settlements[-1]["x"], settlements[-1]["z"]), 6)}]
    mountain_count = 1 + int(random_unit(seed, "mountain-count") * 3.2)
    mountains = []
    for index in range(mountain_count):
        x, z = feature_point(seed, "mountain", index, 0.16)
        base_y = round(max(terrain_height(terrain, x, z), water_level), 2)
        mountains.append({"id": f"{country['countryId']}-mountain-{index + 1}", "name": f"{country['name']} Highland {index + 1}", "x": x, "y": base_y, "z": z, "heightM": round(18 + random_unit(seed, f"mountain:{index}:height") * 42, 2), "radiusM": round(14 + random_unit(seed, f"mountain:{index}:radius") * 30, 1)})
    ports = []
    if country["region"] in {"Velmora Isles", "Nembasa Greenbelt", "Avarra Crescent"} or random_unit(seed, "port") > 0.7:
        x, z = feature_point(seed, "port", 0, 0.05)
        ports.append({"id": f"{country['countryId']}-port-1", "name": f"{country['name']} Maritime Port", "x": x, "y": round(water_level + 0.3, 2), "z": z, "capacity": "large" if random_unit(seed, "port-size") > 0.6 else "small"})
    return {"rivers": rivers, "lakes": lakes, "mountains": mountains, "settlements": settlements, "roads": roads, "bridges": bridges, "railways": railways, "airports": airports, "ports": ports, "militaryBases": military_bases, "metroLines": [{"id": f"{country['countryId']}-metro-1", "status": "planned", "serves": [settlement["id"] for settlement in settlements[: min(3, len(settlements))]]}], "airRoutes": [{"id": f"{country['countryId']}-air-route-1", "status": "regional", "airportId": airports[0]["id"] if airports else None}], "shipLanes": [{"id": f"{country['countryId']}-ship-lane-1", "status": "regional", "portId": ports[0]["id"] if ports else None}], "submarineZones": [{"id": f"{country['countryId']}-submarine-zone-1", "status": "charted", "depthM": round(40 + random_unit(seed, "sub-depth") * 180, 1)}]}


def terrain_for(country: dict[str, Any]) -> dict[str, Any]:
    seed = stable_seed(country["countryId"])
    biome, roughness, moisture = REGION_BIOMES.get(
        country["region"], ("temperate", 0.2, 0.5)
    )
    phase_x = (seed % 997) / 997.0 * math.tau
    phase_z = ((seed >> 11) % 991) / 991.0 * math.tau
    slope = (((seed >> 21) % 2001) - 1000) / 1000.0 * 0.16
    heights: list[float] = []
    for z in range(GRID_SIZE):
        for x in range(GRID_SIZE):
            nx = x / (GRID_SIZE - 1) - 0.5
            nz = z / (GRID_SIZE - 1) - 0.5
            broad = math.sin(nx * 3.1 + phase_x) * 0.55 + math.cos(nz * 2.7 + phase_z) * 0.45
            detail = math.sin((nx + nz) * 10.0 + phase_x * 0.7) * 0.12
            basin = -(nx * nx + nz * nz) * (0.25 if "basin" in biome else 0.08)
            elevation_scale = 14.0 + roughness * 58.0
            height = 4.0 + broad * elevation_scale + detail * (4.0 + roughness * 12.0) + basin * elevation_scale + nx * slope * 12.0
            heights.append(round(max(-1.5, min(86.0, height)), 4))
    terrain = {
        "schemaVersion": 1,
        "countryId": country["countryId"],
        "mapKey": country["mapKey"],
        "biome": biome,
        "seed": seed,
        "grid": {
            "columns": GRID_SIZE,
            "rows": GRID_SIZE,
            "cellSizeM": 16.0,
            "originXM": -128.0,
            "originZM": -128.0,
            "heightsM": heights,
        },
        "surface": {
            "friction": round(0.72 + moisture * 0.22, 3),
            "waterLevelM": round(0.34 + moisture * 0.12, 3),
            "roadClearanceM": 0.08,
        },
    }
    terrain["features"] = features_for(country, terrain)
    return terrain


def template_for(country: dict[str, Any], terrain: dict[str, Any]) -> dict[str, Any]:
    slug = country["name"].lower()
    return {
        "schemaVersion": 1,
        "countryId": country["countryId"],
        "name": country["name"],
        "status": "playable",
        "completion": "template",
        "templatePolicy": {
            "generated": True,
            "requiresReview": True,
            "placeholderContentAllowed": True,
        },
        "terrain": {
            "asset": f"data/country-content/{slug}/terrain.json",
            "mapKey": country["mapKey"],
            "biome": terrain["biome"],
        },
        "spawn": {"x": 0.0, "y": 1.2, "z": 0.0, "headingRad": 0.0},
        "settlements": terrain["features"]["settlements"],
        "infrastructure": terrain["features"],
        "missions": [
            {"id": f"{country['countryId']}-intro", "title": f"Welcome to {country['name']}", "status": "template", "placeholder": True},
            {"id": f"{country['countryId']}-route", "title": "Cross-Region Route", "status": "template", "placeholder": True},
        ],
        "systems": {"weatherProfile": "template", "peopleProfile": country["peopleProfileId"], "trafficProfile": "template"},
        "nextSteps": ["replace placeholder settlements", "author terrain landmarks", "author missions and NPC schedules"],
    }


def write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def main() -> int:
    args = parse_args()
    world_path = args.world if args.world.is_absolute() else ROOT / args.world
    output = args.output if args.output.is_absolute() else ROOT / args.output
    frontend_output = args.frontend_output if args.frontend_output.is_absolute() else ROOT / args.frontend_output
    world = json.loads(world_path.read_text(encoding="utf-8"))
    countries = world["countries"]
    selected = countries if args.include_starter else countries[1:]
    generated = 0
    skipped = 0
    for country in selected:
        directory = output / country["name"].lower()
        terrain_path = directory / "terrain.json"
        template_path = directory / "content-template.json"
        frontend_terrain_path = frontend_output / f"{country['name'].lower()}.json"
        if not args.force and terrain_path.exists() and template_path.exists() and frontend_terrain_path.exists():
            skipped += 1
            continue
        terrain = terrain_for(country)
        write_json(terrain_path, terrain)
        write_json(template_path, template_for(country, terrain))
        write_json(frontend_terrain_path, terrain)
        generated += 1
    manifest = {
        "schemaVersion": 1,
        "generatedForWorld": world["worldId"],
        "countryCount": len(countries),
        "generatedCountryCount": len(selected),
        "starterExcluded": not args.include_starter,
        "packages": [country["countryId"] for country in selected],
    }
    write_json(output / "manifest.json", manifest)
    print(f"Generated {generated} country packages; skipped={skipped}; selected={len(selected)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
