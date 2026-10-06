#!/usr/bin/env python3
"""Generate the native state-system hierarchy for the 120-country world."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]

def slug(value: str) -> str:
    return re.sub(r"[^a-z0-9]+", "-", value.lower()).strip("-")

def write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

def main() -> int:
    parser = argparse.ArgumentParser(description="Generate the 120-country state hierarchy.")
    parser.add_argument("--world", type=Path, default=ROOT / "data/world.json")
    parser.add_argument("--content", type=Path, default=ROOT / "data/country-content")
    parser.add_argument("--output", type=Path, default=ROOT / "data/state-system")
    parser.add_argument("--force", action="store_true")
    args = parser.parse_args()
    world = json.loads(args.world.read_text(encoding="utf-8"))
    regions = {region["name"]: region for region in world["regions"]}
    states: list[dict[str, Any]] = [{"stateId": world["worldId"], "parentStateId": "", "countryId": "", "name": world["name"], "level": "world", "population": 0, "playable": True}]
    for region in world["regions"]:
        states.append({"stateId": f"region-{region['regionId']}", "parentStateId": world["worldId"], "countryId": "", "name": region["name"], "level": "region", "population": 0, "playable": True})
    packages = []
    for country in world["countries"]:
        country_id = country["countryId"]
        country_state_id = f"state-{country_id}"
        region = regions[country["region"]]
        states.append({"stateId": country_state_id, "parentStateId": f"region-{region['regionId']}", "countryId": country_id, "name": country["name"], "level": "country", "population": 0, "playable": country["status"] == "playable"})
        terrain_path = args.content / country["name"].lower() / "terrain.json"
        terrain = json.loads(terrain_path.read_text(encoding="utf-8"))
        country_states = [s for s in states if s.get("countryId") == country_id]
        for settlement in terrain["features"]["settlements"]:
            level = "city" if settlement["type"] == "capital" else "settlement"
            state = {"stateId": f"state-{settlement['id']}", "parentStateId": country_state_id, "countryId": country_id, "name": settlement["name"], "level": level, "population": 0, "playable": True}
            states.append(state)
            country_states.append(state)
        package = {"schemaVersion": 1, "worldId": world["worldId"], "countryId": country_id, "stateIds": [s["stateId"] for s in country_states]}
        write_json(args.output / f"{country['name'].lower()}.json", package)
        packages.append(country_id)
    write_json(args.output / "manifest.json", {"schemaVersion": 1, "worldId": world["worldId"], "stateCount": len(states), "countryCount": len(packages), "packages": packages, "states": states})
    print(f"Generated state system: countries={len(packages)} states={len(states)}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
