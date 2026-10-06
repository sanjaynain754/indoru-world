#!/usr/bin/env python3
"""Generate deterministic production transport contracts for all playable countries.

The output is engine-facing data, not final meshes. It connects generated geography
(rivers, roads, bridges, settlements and rail lines) to the C++ simulation bridge and
leaves UE5/desktop rendering free to consume the stable IDs and polylines.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_WORLD = ROOT / "data" / "world.json"
DEFAULT_CONTENT = ROOT / "data" / "country-content"
DEFAULT_OUTPUT = ROOT / "data" / "transport-network"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate 120-country road, rail and river networks.")
    parser.add_argument("--world", type=Path, default=DEFAULT_WORLD)
    parser.add_argument("--content", type=Path, default=DEFAULT_CONTENT)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--force", action="store_true")
    return parser.parse_args()


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def normalize_points(points: list[dict[str, float]]) -> list[dict[str, float]]:
    return [{"x": float(point["x"]), "y": float(point["y"]), "z": float(point["z"])} for point in points]


def network_for(country: dict[str, Any], terrain: dict[str, Any]) -> dict[str, Any]:
    features = terrain["features"]
    country_id = country["countryId"]
    settlements = features["settlements"]

    roads = []
    for index, source in enumerate(features["roads"], 1):
        class_name = source.get("class", "regional")
        roads.append({
            "roadId": source["id"],
            "class": "arterial" if class_name == "highway" else "collector",
            "lanes": 4 if class_name == "highway" else 2,
            "speedLimitKph": 90 if class_name == "highway" else 55,
            "access": {"vehicles": True, "pedestrians": True, "emergency": True},
            "points": normalize_points(source["points"]),
            "connectedSettlementIds": [settlements[0]["id"], settlements[min(index, len(settlements) - 1)]["id"]],
        })

    bridges = []
    for source in features["bridges"]:
        bridges.append({
            "bridgeId": source["id"],
            "roadId": source["roadId"],
            "riverId": source["riverId"],
            "lengthM": float(source["lengthM"]),
            "loadClass": "heavy-vehicle",
            "supportsTrain": False,
            "x": float(source["x"]),
            "y": float(source["y"]),
            "z": float(source["z"]),
        })

    lines = []
    stations = []
    for source in features["railways"]:
        line_id = source["id"]
        lines.append({
            "lineId": line_id,
            "mode": "intercity-rail",
            "electrification": "future-ready",
            "trackGaugeMm": 1435,
            "maxSpeedKph": 180,
            "points": normalize_points(source["points"]),
        })
    for settlement in settlements:
        stations.append({
            "stationId": f"{country_id}-{settlement['id']}-station",
            "settlementId": settlement["id"],
            "name": f"{settlement['name']} Central Station",
            "service": "intercity",
            "x": float(settlement["x"]),
            "y": float(settlement["y"]),
            "z": float(settlement["z"]),
        })

    rivers = []
    for source in features["rivers"]:
        rivers.append({
            "riverId": source["id"],
            "navigable": float(source["widthM"]) >= 12.0,
            "widthM": float(source["widthM"]),
            "transportMode": "river-freight" if float(source["widthM"]) >= 12.0 else "waterway",
            "points": normalize_points(source["points"]),
        })

    return {
        "schemaVersion": 1,
        "countryId": country_id,
        "mapKey": country["mapKey"],
        "seed": terrain["seed"],
        "sourceTerrain": f"data/country-content/{country['name'].lower()}/terrain.json",
        "roadNetwork": {"segments": roads, "bridges": bridges},
        "railNetwork": {"lines": lines, "stations": stations, "trainService": {"enabled": True, "rollingStockProfile": "regional-electric-template"}},
        "riverNetwork": {"rivers": rivers, "riverBridges": [bridge["bridgeId"] for bridge in bridges]},
        "simulationHooks": {
            "cityInfrastructure": True,
            "residentialAccess": True,
            "trafficSimulation": True,
            "massTransitSimulation": True,
            "waterTrafficSimulation": any(river["navigable"] for river in rivers),
        },
    }


def main() -> int:
    args = parse_args()
    world_path = args.world if args.world.is_absolute() else ROOT / args.world
    content_root = args.content if args.content.is_absolute() else ROOT / args.content
    output_root = args.output if args.output.is_absolute() else ROOT / args.output
    world = read_json(world_path)
    packages = []
    road_count = rail_count = river_count = bridge_count = station_count = 0
    for country in world["countries"]:
        slug = country["name"].lower()
        terrain_path = content_root / slug / "terrain.json"
        if not terrain_path.is_file():
            raise FileNotFoundError(f"Missing terrain package for {country['countryId']}: {terrain_path}")
        output_path = output_root / f"{slug}.json"
        if output_path.exists() and not args.force:
            network = read_json(output_path)
        else:
            network = network_for(country, read_json(terrain_path))
            write_json(output_path, network)
        packages.append(country["countryId"])
        road_count += len(network["roadNetwork"]["segments"])
        bridge_count += len(network["roadNetwork"]["bridges"])
        rail_count += len(network["railNetwork"]["lines"])
        station_count += len(network["railNetwork"]["stations"])
        river_count += len(network["riverNetwork"]["rivers"])
    manifest = {
        "schemaVersion": 1,
        "worldId": world["worldId"],
        "countryCount": len(packages),
        "packages": packages,
        "totals": {"roads": road_count, "bridges": bridge_count, "railLines": rail_count, "railStations": station_count, "rivers": river_count},
    }
    write_json(output_root / "manifest.json", manifest)
    print(f"Generated transport networks: countries={len(packages)} roads={road_count} bridges={bridge_count} railLines={rail_count} stations={station_count} rivers={river_count}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
