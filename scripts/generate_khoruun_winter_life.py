#!/usr/bin/env python3
"""Generate the reusable cold-climate living layer for all 18 Khoruun countries."""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]

BAND_RULES: dict[str, dict[str, Any]] = {
    "polar-highland": {
        "buildingProfile": "polar-insulated",
        "heating": ["district-heat", "backup-boiler", "heated-entry"],
        "snow": ["roof-snow-load", "covered-walkway", "snow-dump-zone", "blizzard-shelter"],
        "mobility": ["tracked-service-vehicle", "snow-cleared-priority-route", "heated-bus-stop"],
        "winterIntensity": 1.0,
    },
    "temperate-plateau": {
        "buildingProfile": "cold-plateau-insulated",
        "heating": ["district-heat", "heat-pump", "sealed-thermal-envelope"],
        "snow": ["roof-snow-load", "wind-shelter", "snow-dump-zone", "ice-grit-depot"],
        "mobility": ["all-weather-road", "heated-bus-stop", "winter-rail-maintenance"],
        "winterIntensity": 0.7,
    },
    "arid-canyon": {
        "buildingProfile": "cold-night-canyon",
        "heating": ["high-efficiency-boiler", "thermal-mass", "sealed-thermal-envelope"],
        "snow": ["cold-night-shelter", "flash-flood-clearance", "dust-and-ice-depot"],
        "mobility": ["all-weather-road", "heated-bus-stop", "canyon-closure-detour"],
        "winterIntensity": 0.35,
    },
}


def read(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8"))


def write(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def build_package(country: dict[str, Any], seed: int) -> dict[str, Any]:
    band = country["weather"]["globalClimateBand"]
    rules = BAND_RULES[band]
    name = country["name"]
    intensity = rules["winterIntensity"]
    return {
        "schemaVersion": 1,
        "countryId": country["countryId"],
        "countryName": name,
        "regionId": "region-khoruun-reach",
        "source": "deterministic-cold-climate-baseline",
        "globe": {"latitudeDegrees": country["globePlacement"]["latitudeDegrees"], "climateBand": band, "weatherPartition": country["weather"]["weatherPartition"]},
        "winterDesign": {
            "buildingProfile": rules["buildingProfile"],
            "winterIntensity": intensity,
            "thermalEnvelope": ["continuous-insulation", "air-sealed-envelope", "triple-pane-or-equivalent-windows", "mechanical-fresh-air", "pipe-freeze-protection"],
            "heatingSystems": rules["heating"],
            "snowAndWindSystems": rules["snow"],
            "mobilitySystems": rules["mobility"],
            "publicRealm": ["sun-facing-rest-area", "wind-sheltered-square", "covered-entry-canopies", "winter-lighting", "snow-storage-kept-away-from-drainage"],
        },
        "residentialDistricts": [
            {"id": f"{country['countryId']}-heated-courtyard", "type": "heated-courtyard-colony", "homes": 48 + seed % 24, "features": ["shared-heating-loop", "covered-entry", "drying-room", "child-safe-snow-play"]},
            {"id": f"{country['countryId']}-wind-shelter-colony", "type": "wind-sheltered-family-colony", "homes": 32 + (seed // 7) % 20, "features": ["windbreak-blocks", "heated-footpath", "grocery-nearby", "clinic-route"]},
            {"id": f"{country['countryId']}-service-housing", "type": "worker-and-service-housing", "homes": 18 + (seed // 13) % 12, "features": ["garage-access", "shift-worker-common-room", "emergency-power"]},
        ],
        "shopsAndServices": [
            {"id": f"{country['countryId']}-winter-market", "type": "heated-market", "name": f"{name} Winter Market", "openHours": [7, 22], "delivery": ["snow-road", "rail-freight"], "services": ["groceries", "warm-food", "local-produce", "winter-clothing"]},
            {"id": f"{country['countryId']}-repair-row", "type": "vehicle-and-heating-repair", "name": f"{name} Repair Row", "openHours": [6, 23], "delivery": ["road", "service-van"], "services": ["vehicle-repair", "snow-plow-repair", "boiler-service", "tire-chain-fitting"]},
            {"id": f"{country['countryId']}-health-corner", "type": "clinic-and-pharmacy", "name": f"{name} Health Corner", "openHours": [0, 24], "delivery": ["emergency-road", "medevac"], "services": ["clinic", "pharmacy", "cold-injury-response"]},
            {"id": f"{country['countryId']}-barber-cafe", "type": "barber-and-warm-cafe", "name": f"{name} Warm Corner", "openHours": [8, 21], "delivery": ["pedestrian", "heated-bus"], "services": ["barber", "cafe", "public-heating-lounge"]},
        ],
        "showroom": {
            "id": f"{country['countryId']}-winter-showroom",
            "type": "winter-mobility-and-home-showroom",
            "name": f"{name} Snowline Showroom",
            "exhibits": ["insulated-home-module", "district-heating-control", "snow-clearing-vehicle", "tracked-rescue-vehicle", "winter-rail-car", "cold-weather-clothing"],
            "demoRoute": ["heated-market", "snow-depot", "mountain-pass-gate", "remote-village"],
            "interactiveActions": ["compare-heating-cost", "test-snow-traction", "plan-winter-delivery", "inspect-insulation", "dispatch-rescue-vehicle"],
        },
        "weatherGameplay": {
            "hazardInputs": country["weather"]["hazards"],
            "responses": ["clear-priority-roads", "open-warm-shelter", "reroute-market-delivery", "protect-water-pipes", "delay-or-resume-rail", "dispatch-medical-rescue"],
            "npcLoops": ["morning-heating-check", "market-delivery", "school-safe-route", "snow-clearing-shift", "evening-warm-cafe", "storm-shelter-callout"],
        },
        "assetPolicy": {"status": "generated-contract", "requiresAuthoredMeshes": True, "requiresWinterMaterials": True, "requiresGameplayHooks": True},
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--baseline", type=Path, default=ROOT / "data/region-baselines/khoruun-reach.json")
    parser.add_argument("--output", type=Path, default=ROOT / "data/country-content")
    args = parser.parse_args()
    baseline = read(args.baseline)
    countries = baseline["countries"]
    if len(countries) != 18:
        raise SystemExit(f"expected 18 Khoruun countries, found {len(countries)}")
    for index, country in enumerate(countries):
        seed = 21_001 + index * 7_919
        write(args.output / country["name"].lower() / "winter-life.json", build_package(country, seed))
    print(f"Generated winter-life packages: countries={len(countries)} homesets=3 shops=4 showroom=1 each")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
