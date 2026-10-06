#!/usr/bin/env python3
import json, sys
from pathlib import Path

p = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).parents[1] / "data/ue5/20-country-world-partition-manifest.json"
d = json.loads(p.read_text(encoding="utf-8"))
errors=[]

if len(d["countries"]) != 20:
    errors.append(f"Expected 20 countries, got {len(d['countries'])}")
if d["world"]["simulatedPopulation"] != 10_000_000:
    errors.append("World simulated population must be exactly 10,000,000")

pop=sum(c["population"] for c in d["countries"])
if pop != 10_000_000:
    errors.append(f"Country population total is {pop}, expected 10,000,000")

ids=set()
for c in d["countries"]:
    if c["countryId"] in ids:
        errors.append(f"Duplicate country id: {c['countryId']}")
    ids.add(c["countryId"])
    if c["region"]["id"] != "avr":
        errors.append(f"{c['countryName']} is not in AVR")
    if not c["flag"]["unique"]:
        errors.append(f"{c['countryName']} flag is not unique")
    if len(c["locations"]) != 7:
        errors.append(f"{c['countryName']} must have 1 capital + 3 cities + 3 villages")
    for loc in c["locations"]:
        if loc["flagProtocol"]["local"] != c["flag"]["assetId"]:
            errors.append(f"Wrong local flag on {loc['locationId']}")
        if "Weather" not in loc["dataLayers"]:
            errors.append(f"Weather layer missing on {loc['locationId']}")

print("UE5_WORLD_MANIFEST_PASS" if not errors else "UE5_WORLD_MANIFEST_FAIL")
for e in errors:
    print("ERROR:", e)
raise SystemExit(1 if errors else 0)
