#!/usr/bin/env python3
"""Generate Khoruun Reach's reviewable production baseline.

This creates data contracts only. Concept-art images remain references and are not
silently treated as canonical geography or shipped assets.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
WEATHER_BY_IDENTITY = {
    "Red canyon federation": {"climate": "arid-canyon", "seasons": ["cool-dry", "hot-dry", "storm-transition", "cold-night"], "hazards": ["flash-flood", "dust-storm", "rockfall", "extreme-heat"], "modifiers": {"roadGrip": [0.75, 0.95], "visibilityM": [80, 18000], "npcActivity": [0.45, 1.0]}, "responses": ["close-canyon-road", "activate-flood-warning", "reroute-freight"]},
    "Salt flats and solar energy": {"climate": "salt-desert", "seasons": ["dry-wind", "peak-heat", "monsoon-edge", "cold-clear"], "hazards": ["salt-corrosion", "dust-storm", "heat-haze", "flash-flood"], "modifiers": {"roadGrip": [0.65, 0.98], "visibilityM": [120, 22000], "npcActivity": [0.35, 1.0]}, "responses": ["protect-solar-assets", "close-salt-track", "reroute-service-convoy"]},
    "Cold border plateau": {"climate": "cold-plateau", "seasons": ["deep-winter", "thaw", "short-summer", "freeze-up"], "hazards": ["blizzard", "black-ice", "avalanche", "permafrost-heave"], "modifiers": {"roadGrip": [0.25, 0.98], "visibilityM": [20, 20000], "npcActivity": [0.2, 1.0]}, "responses": ["snow-clear-priority-route", "open-warm-shelter", "delay-rail-service"]},
    "Canyon railways and mining towns": {"climate": "high-desert", "seasons": ["dry-winter", "hot-summer", "thunderstorm", "cold-evening"], "hazards": ["rockfall", "mine-subsidence", "dust-storm", "flash-flood"], "modifiers": {"roadGrip": [0.55, 0.98], "visibilityM": [60, 21000], "npcActivity": [0.4, 1.0]}, "responses": ["inspect-tunnel", "hold-freight-train", "close-blast-zone"]},
}
DEFAULT_WEATHER = {"climate": "continental-frontier", "seasons": ["cold-dry", "hot-dry", "storm-season", "cool-transition"], "hazards": ["dust-storm", "flash-flood", "heatwave", "cold-snap"], "modifiers": {"roadGrip": [0.5, 0.98], "visibilityM": [50, 22000], "npcActivity": [0.3, 1.0]}, "responses": ["issue-weather-alert", "reroute-public-transport", "open-emergency-depot"]}


def read(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def write(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def map_slices(country: dict[str, Any], terrain: dict[str, Any]) -> list[dict[str, Any]]:
    slug = country["name"].lower()
    features = terrain.get("features", {})
    return [
        {"mapId": f"{slug}-capital-core", "role": "capital-and-civic-core", "layers": ["border", "capital", "city", "roads", "rail", "weather", "services"], "sourceFeatures": ["settlements", "roads", "railways"]},
        {"mapId": f"{slug}-resource-frontier", "role": "industry-or-resource-frontier", "layers": ["border", "settlements", "roads", "terrain", "weather", "industry"], "sourceFeatures": ["mountains", "settlements", "roads", "militaryBases"]},
        {"mapId": f"{slug}-water-and-corridor", "role": "river-lake-or-coastal-corridor", "layers": ["border", "rivers", "lakes", "bridges", "ports", "roads", "weather"], "sourceFeatures": ["rivers", "lakes", "bridges", "ports", "shipLanes"]},
        {"mapId": f"{slug}-remote-settlements", "role": "remote-villages-and-wilderness", "layers": ["border", "villages", "terrain", "emergency-routes", "weather"], "sourceFeatures": ["settlements", "mountains", "roads", "submarineZones"]},
    ]


def build_country(country: dict[str, Any], terrain: dict[str, Any]) -> dict[str, Any]:
    identity = country.get("identity", "")
    weather = dict(WEATHER_BY_IDENTITY.get(identity, DEFAULT_WEATHER))
    ordinal = int(country["countryId"].split("-")[-1]) - 21
    latitude = 34.0 + (ordinal % 6) * 5.4 + (ordinal // 6) * 0.8
    longitude = 24.0 + (ordinal // 6) * 25.0 + (ordinal % 6) * 2.2
    weather["countryId"] = country["countryId"]
    weather["simulationEffects"] = ["traffic", "visibility", "road-grip", "rail-delay", "river-level", "npc-schedules", "emergency-calls", "power-demand", "asset-wear"]
    weather["dailyCycle"] = {"forecastHours": 24, "updateMinutes": 15, "seededBy": ["countryId", "terrainSeed", "simulationDay"]}
    return {
        "countryId": country["countryId"],
        "name": country["name"],
        "regionId": "region-khoruun-reach",
        "identity": identity,
        "globePlacement": {"latitudeDegrees": round(latitude, 4), "longitudeDegrees": round(longitude, 4), "coordinateSystem": "WGS84-like fictional globe", "pinAnchor": "capital", "datelineSafe": True},
        "mapScale": {"unit": "km", "worldExtent": [0, 100, 0, 100], "cellSize": 2.0},
        "mapSlices": map_slices(country, terrain),
        "visualLayers": ["country-border", "capital-marker", "city-marker", "village-marker", "road-network", "rail-network", "river-and-waterway", "bridge-and-tunnel", "mountains", "weather-overlay", "compass", "scale-bar"],
        "imageReferences": [{"assetId": f"concept-khoruun-{country['name'].lower()}", "status": "reference-only", "path": f"assets/concept/khoruun/{country['name'].lower()}.png", "canonical": False}],
        "terrainContract": {"source": f"data/country-content/{country['name'].lower()}/terrain.json", "requiredFeatures": ["mountains", "rivers", "roads", "railways", "bridges", "settlements"], "noBuildZones": ["protected-wilderness", "unstable-slope", "salt-flat-core"]},
        "weather": weather,
        "productionAcceptance": {"borderVerified": False, "locationNamesVerified": True, "imagePlacementVerified": False, "weatherSimulationVerified": False, "transportGameplayVerified": False, "nativeStreamingVerified": False},
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--world", type=Path, default=ROOT / "data/world.json")
    parser.add_argument("--content", type=Path, default=ROOT / "data/country-content")
    parser.add_argument("--output", type=Path, default=ROOT / "data/region-baselines/khoruun-reach.json")
    args = parser.parse_args()
    world = read(args.world)
    countries = [c for c in world["countries"] if c.get("region") == "Khoruun Reach"]
    if len(countries) != 18:
        raise SystemExit(f"expected 18 Khoruun countries, found {len(countries)}")
    records = []
    for country in countries:
        terrain = read(args.content / country["name"].lower() / "terrain.json")
        records.append(build_country(country, terrain))
    output = {"schemaVersion": 2, "worldId": world["worldId"], "regionId": "region-khoruun-reach", "regionName": "Khoruun Reach", "buildOrder": 2, "contentState": "baseline-design", "platformTarget": ["desktop-gaming-pc", "playstation-5"], "canonicalImagePolicy": "concept images are references only until approved placement assets exist", "globe": {"projection": "equirectangular-authored-to-unit-sphere", "pinAnimation": "shortest-longitude-smoothstep", "rotationDurationSeconds": 0.8, "climateBands": [{"id": "polar-highland", "latitudeMin": 64.0, "latitudeMax": 90.0, "visual": "snow-and-ice", "hazards": ["blizzard", "avalanche"]}, {"id": "temperate-plateau", "latitudeMin": 48.0, "latitudeMax": 64.0, "visual": "cold-plateau-and-forest", "hazards": ["black-ice", "rockfall"]}, {"id": "arid-canyon", "latitudeMin": 34.0, "latitudeMax": 48.0, "visual": "canyon-and-salt-flat", "hazards": ["dust-storm", "flash-flood", "heatwave"]}]}, "mapCount": len(records) * 4, "countryCount": len(records), "countries": records}
    write(args.output, output)
    print(f"Generated Khoruun baseline: countries={len(records)} maps={len(records) * 4} weatherProfiles={len(records)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
