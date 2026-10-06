#!/usr/bin/env python3
"""Validate engine-neutral region content manifests against canonical registries."""
from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from build_avarra_baseline import build_manifest  # noqa: E402

MANIFEST_PATH = ROOT / "data/region-baselines/avarra-crescent.json"
REQUIRED_CITY_SERVICES = {
    "housing_districts", "public_market", "grocery_and_general_stores", "barber_shop",
    "clinic", "pharmacy", "school", "police_station", "fire_station",
    "vehicle_mechanic", "potable_water", "electricity", "sanitation_and_sewer",
    "solid_waste_collection", "public_transit_stop",
}
REQUIRED_VILLAGE_SERVICES = {
    "safe_housing", "potable_water_point", "electricity_or_microgrid",
    "toilets_and_sanitation", "primary_school", "community_hall",
    "clinic_outreach", "emergency_response_access", "all_weather_road",
    "weekly_market", "mobile_barber_service",
}


def _resolved_services(profile_id: str, profiles: dict, seen: set[str] | None = None) -> set[str]:
    seen = seen or set()
    if profile_id in seen or profile_id not in profiles:
        return set()
    seen.add(profile_id)
    profile = profiles[profile_id]
    services = set(profile.get("additional", []))
    parent = profile.get("inherits")
    if parent:
        services |= _resolved_services(parent, profiles, seen)
    return services


def validate_manifest(manifest: dict) -> list[str]:
    errors: list[str] = []
    canonical = json.loads((ROOT / "data/countries.json").read_text(encoding="utf-8"))
    expected_countries = {
        c["countryId"]: c["name"] for c in canonical if c.get("region") == "Avarra Crescent"
    }
    roster = {c.get("countryId"): c.get("name") for c in manifest.get("countries", [])}
    if roster != expected_countries:
        errors.append("country roster differs from canonical Avarra country registry")

    settlements = manifest.get("settlements", [])
    ids = [s.get("settlementId") for s in settlements]
    if len(ids) != len(set(ids)):
        errors.append("settlement IDs are not unique")
    names = [s.get("name") for s in settlements]
    if len(names) != len(set(names)):
        errors.append("settlement names are not unique within Avarra")
    country_ids = set(expected_countries)
    for settlement in settlements:
        if settlement.get("region") != "Avarra Crescent" or settlement.get("regionId") != "region-avarra-crescent":
            errors.append(f"settlement {settlement.get('name')} has the wrong region")
        if settlement.get("countryId") not in country_ids:
            errors.append(f"settlement {settlement.get('name')} has an unknown Avarra owner")
        if settlement.get("flagRef") != f"country:{settlement.get('countryId')}":
            errors.append(f"settlement {settlement.get('name')} has a mismatched flag reference")
        if settlement.get("assignmentReview") != "proposed":
            errors.append(f"settlement {settlement.get('name')} must remain marked for review")

    covered = {s.get("countryId") for s in settlements}
    missing = sorted(country_ids - covered)
    if missing:
        errors.append(f"countries without a named settlement: {', '.join(missing)}")

    profiles = manifest.get("facilityProfiles", {})
    for profile_id in ("capital", "major_city", "normal_city"):
        services = _resolved_services(profile_id, profiles)
        missing_services = sorted(REQUIRED_CITY_SERVICES - services)
        if missing_services:
            errors.append(f"{profile_id} lacks city services: {', '.join(missing_services)}")
    village_services = _resolved_services("village", profiles)
    missing_village_services = sorted(REQUIRED_VILLAGE_SERVICES - village_services)
    if missing_village_services:
        errors.append(f"village profile lacks baseline services: {', '.join(missing_village_services)}")

    settlement_ids = set(ids)
    connectivity = manifest.get("connectivity", {})
    if set(connectivity.get("roadAccessSettlementIds", [])) != settlement_ids:
        errors.append("road access must explicitly cover every settlement exactly")
    for key in ("railLines", "waterRoutes", "airLinks"):
        for route in connectivity.get(key, []):
            endpoints = route.get("settlementIds", [])
            if len(endpoints) < 2 or not set(endpoints) <= settlement_ids:
                errors.append(f"{key} route {route.get('name')} has missing/unknown endpoints")
    for airport in connectivity.get("airportRoles", []):
        if airport.get("settlementId") not in settlement_ids:
            errors.append(f"airport role references unknown settlement {airport.get('settlementId')}")

    counts = manifest.get("settlementCounts", {})
    actual = {
        "majorCities": sum(s.get("type") in ("capital", "major_city") for s in settlements),
        "normalCities": sum(s.get("type") == "normal_city" for s in settlements),
        "villages": sum(s.get("type") == "village" for s in settlements),
        "total": len(settlements),
        "countriesCovered": len(covered),
    }
    if counts != actual:
        errors.append(f"settlementCounts mismatch: recorded={counts}, actual={actual}")

    serialized = json.dumps(manifest, ensure_ascii=False).casefold()
    if "country-indoru" in serialized:
        errors.append("legacy single-country owner is not allowed in the Avarra manifest")
    if "coming soon" in serialized or "coming_soon" in serialized:
        errors.append("player-facing Coming Soon label/state is not allowed in this manifest")
    if manifest.get("runtimeIntegrated") is not False:
        errors.append("design manifest must not claim integration into a playable runtime")
    if manifest.get("contentState") != "baseline-design":
        errors.append("manifest must remain explicitly marked as baseline design")
    return errors


def main() -> int:
    if not MANIFEST_PATH.exists():
        print(f"FAIL: missing {MANIFEST_PATH.relative_to(ROOT)}", file=sys.stderr)
        return 1
    manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
    errors = validate_manifest(manifest)
    if manifest != build_manifest():
        errors.append("generated manifest is stale; run scripts/build_avarra_baseline.py")
    if errors:
        for error in errors:
            print(f"FAIL: {error}", file=sys.stderr)
        return 1
    counts = manifest["settlementCounts"]
    print(f"PASS: {counts['countriesCovered']} countries; {counts['total']} settlements; city/village services and connectivity validated")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
