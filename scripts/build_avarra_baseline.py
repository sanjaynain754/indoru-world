#!/usr/bin/env python3
"""Build the first, reviewable Avarra region content baseline.

This creates an engine-neutral design manifest. It deliberately does not mark
unbuilt countries playable or alter the legacy global settlement registry.
"""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REGION = "Avarra Crescent"
REGION_ID = "region-avarra-crescent"

CITY_OWNERS = {
    "Navaar": "country-001",
    "Solmera": "country-002",
    "Mirqara Prime": "country-003",
    "Arela": "country-004",
    "Havora": "country-005",
    "Orela": "country-006",
    "Vareka": "country-007",
    "Cavoren": "country-008",
    "Jalenen": "country-009",
    "Qorinen": "country-010",
    "Xarinen": "country-011",
    "Evarais": "country-012",
    "Lurais": "country-013",
    "Sorenis": "country-014",
    "Zavelis": "country-015",
    "Girelor": "country-016",
}

VILLAGE_OWNERS = {
    "Ashagaon": "country-001",
    "Himagaon": "country-002",
    "Oshagaon": "country-003",
    "Vinagaon": "country-004",
    "Cirapur": "country-005",
    "Jorapur": "country-006",
    "Qumapur": "country-007",
    "Xirapur": "country-008",
    "Eshavale": "country-009",
    "Lumavale": "country-010",
    "Savavale": "country-011",
    "Ziravale": "country-012",
    "Garonadi": "country-013",
    "Nalanadi": "country-014",
    "Ushanadi": "country-015",
    "Belakhet": "country-016",
}

CITY_ROLES = {
    "Navaar": "River capital, civic administration and national trade hub",
    "Solmera": "Sun-coast finance, media and regional passenger gateway",
    "Mirqara Prime": "Inland-sea port, wholesale markets and freight exchange",
    "Arela": "Terraced-farm service centre with a restored hill citadel",
    "Havora": "Western coastal city with a working harbor and beach districts",
    "Orela": "Delta city shaped by canals, river freight and floodable parks",
    "Vareka": "Limestone highland city with quarry, craft and construction trades",
    "Cavoren": "Grassland market city serving horse, livestock and grain routes",
    "Jalenen": "Industrial river-corridor city with rail yards and worker districts",
    "Qorinen": "Temple-valley city balancing heritage streets and civic services",
    "Xarinen": "Rain-fed forest gateway linked by hill roads and trail services",
    "Evarais": "Salt-coast city with fishing markets, cold stores and storm shelters",
    "Lurais": "Lake-basin city with water management, education and observatories",
    "Sorenis": "Desert-edge caravan city with shaded markets and water depots",
    "Zavelis": "Island-facing royal city with ferry piers and coastal promenades",
    "Girelor": "Mountain-pass logistics city connecting roads, rail and freight",
}

VILLAGE_ROLES = {
    "Ashagaon": "Riverbank farming village with a market-day square",
    "Himagaon": "Dry-hill village with a shared water tank and orchard plots",
    "Oshagaon": "Canal-side village with boat landing and fish stalls",
    "Vinagaon": "Terraced farming village with a hill-road service stop",
    "Cirapur": "Coastal craft village with a storm-safe community hall",
    "Jorapur": "Delta village connected by road, footbridge and local ferry",
    "Qumapur": "Quarry foothill village with dust-control and worker safety services",
    "Xirapur": "Grassland village with a livestock market and veterinary access",
    "Eshavale": "River-corridor village with a bus stop and repair workshop",
    "Lumavale": "Temple-valley village with a heritage square and school",
    "Savavale": "Forest-edge village with firebreaks and hill-road access",
    "Ziravale": "Fishing village with a salt-market shed and cyclone refuge",
    "Garonadi": "Lake village with landing steps and water-quality monitoring",
    "Nalanadi": "Caravan-route village with a shaded rest yard and water point",
    "Ushanadi": "Island-facing village with a ferry pier and boat-repair shed",
    "Belakhet": "Mountain-pass village with a road-maintenance and rescue post",
    "Marenvale": "Blue-cliff grove village with slope-safe footpaths",
    "Meskopur": "Free-trade port village with a small customs and market quay",
    "Arqakhet": "Volcanic-foothill village with hazard shelters and mineral-work safety",
    "Vensarpara": "Southern-bay village with a fishing pier and naval supply access",
}

CITY_SERVICES = [
    "housing_districts", "public_market", "grocery_and_general_stores",
    "barber_shop", "clothing_and_personal_care", "restaurants_and_food_stalls",
    "clinic", "pharmacy", "school", "police_station", "fire_station",
    "vehicle_mechanic", "fuel_or_charging", "potable_water", "electricity",
    "sanitation_and_sewer", "solid_waste_collection", "storm_drainage",
    "public_transit_stop", "park_or_public_square", "civic_service_counter",
]

FACILITY_PROFILES = {
    "capital": {
        "inherits": "major_city",
        "additional": ["national_government_quarter", "teaching_hospital", "university", "international_airport", "central_rail_terminal", "river_ferry_terminal", "intercity_bus_terminal", "freight_yard"],
    },
    "major_city": {
        "inherits": "normal_city",
        "additional": ["regional_hospital", "college_or_training_centre", "rail_or_ferry_terminal", "intercity_bus_terminal", "freight_market"],
    },
    "normal_city": {
        "inherits": None,
        "additional": CITY_SERVICES,
    },
    "village": {
        "inherits": None,
        "additional": ["safe_housing", "potable_water_point", "electricity_or_microgrid", "toilets_and_sanitation", "waste_collection_point", "primary_school", "community_hall", "clinic_outreach", "emergency_response_access", "all_weather_road", "bus_or_shared_van_stop", "weekly_market", "mobile_barber_service", "grocery_kiosk", "small_repairs", "drainage_and_flood_route"],
    },
}

DISTRICTS = {
    "capital": ["civic_core", "riverfront", "historic_market", "mixed_residential", "employment_district", "transit_hub", "parks_and_public_spaces"],
    "major_city": ["commercial_core", "working_waterfront_or_market", "mixed_residential", "employment_district", "transit_hub", "public_green"],
    "normal_city": ["town_centre", "local_market", "residential_neighbourhood", "workshop_or_employment_row", "school_and_clinic", "public_green"],
    "village": ["main_street", "homes_and_yards", "local_working_land", "school_and_community_hall", "market_or_landing", "shared_green"],
}

NEW_VILLAGES = [
    {"settlementId": "settlement-202", "name": "Marenvale", "countryId": "country-017", "countryName": "Ilvarra", "type": "village", "populationTier": "small"},
    {"settlementId": "settlement-203", "name": "Meskopur", "countryId": "country-018", "countryName": "Meskora", "type": "village", "populationTier": "small"},
    {"settlementId": "settlement-204", "name": "Arqakhet", "countryId": "country-019", "countryName": "Arqessa", "type": "village", "populationTier": "small"},
    {"settlementId": "settlement-205", "name": "Vensarpara", "countryId": "country-020", "countryName": "Venqara", "type": "village", "populationTier": "small"},
]


def slug(value: str) -> str:
    return re.sub(r"[^a-z0-9]+", "-", value.casefold()).strip("-")


def build_manifest() -> dict:
    countries = json.loads((ROOT / "data/countries.json").read_text(encoding="utf-8"))
    source_doc = json.loads((ROOT / "data/settlements.json").read_text(encoding="utf-8"))
    region_countries = [c for c in countries if c.get("region") == REGION]
    source_settlements = [s for s in source_doc["settlements"] if s.get("region") == REGION]
    if len(region_countries) != 20:
        raise ValueError(f"Expected 20 Avarra countries, found {len(region_countries)}")
    if len(source_settlements) != 32:
        raise ValueError(f"Expected 32 legacy Avarra settlements, found {len(source_settlements)}")

    country_by_id = {c["countryId"]: c for c in region_countries}
    settlements = []
    mapping = CITY_OWNERS | VILLAGE_OWNERS
    for source in source_settlements:
        name = source["name"]
        if name not in mapping:
            raise ValueError(f"No proposed country assignment for existing settlement {name}")
        owner_id = mapping[name]
        owner = country_by_id.get(owner_id)
        if owner is None:
            raise ValueError(f"Settlement {name} maps outside Avarra: {owner_id}")
        kind = source["type"]
        role = CITY_ROLES.get(name) or VILLAGE_ROLES.get(name)
        if role is None:
            raise ValueError(f"No role/profile text for settlement {name}")
        record = {
            "settlementId": source["settlementId"],
            "name": name,
            "countryId": owner_id,
            "countryName": owner["name"],
            "regionId": REGION_ID,
            "region": REGION,
            "type": kind,
            "populationTier": source["populationTier"],
            "role": role,
            "flagRef": f"country:{owner_id}",
            "mapKey": f"{owner['mapKey']}/settlements/{slug(name)}",
            "facilityProfileId": kind,
            "districtArchetypes": DISTRICTS[kind],
            "assignmentReview": "proposed",
        }
        settlements.append(record)

    for source in NEW_VILLAGES:
        owner = country_by_id[source["countryId"]]
        settlements.append({
            **source,
            "regionId": REGION_ID,
            "region": REGION,
            "role": VILLAGE_ROLES[source["name"]],
            "flagRef": f"country:{source['countryId']}",
            "mapKey": f"{owner['mapKey']}/settlements/{slug(source['name'])}",
            "facilityProfileId": "village",
            "districtArchetypes": DISTRICTS["village"],
            "assignmentReview": "proposed",
        })

    roster = [{
        "countryId": c["countryId"],
        "name": c["name"],
        "regionCode": c["regionCode"],
        "identity": c["identity"],
        "flagId": c["flagId"],
        "flagAsset": c["flagAsset"],
        "settlementIds": sorted(s["settlementId"] for s in settlements if s["countryId"] == c["countryId"]),
    } for c in region_countries]

    settlement_ids = {s["settlementId"] for s in settlements}
    return {
        "schemaVersion": 1,
        "worldId": "indoru-world-001",
        "regionId": REGION_ID,
        "regionName": REGION,
        "buildOrder": 1,
        "contentState": "baseline-design",
        "platformTarget": ["desktop-gaming-pc", "playstation-5"],
        "runtimeIntegrated": False,
        "mappingReview": {
            "status": "proposed",
            "note": "Avenra is the confirmed starter country. Existing Avarra city and village ownership beyond that is a proposed distribution by country identity; review before migrating this manifest into the canonical global settlement registry.",
            "legacyAvarraSettlementCount": len(source_settlements),
            "newVillageCount": len(NEW_VILLAGES),
        },
        "countries": roster,
        "settlementCounts": {
            "majorCities": sum(s["type"] in ("capital", "major_city") for s in settlements),
            "normalCities": sum(s["type"] == "normal_city" for s in settlements),
            "villages": sum(s["type"] == "village" for s in settlements),
            "total": len(settlements),
            "countriesCovered": len({s["countryId"] for s in settlements}),
        },
        "facilityProfiles": FACILITY_PROFILES,
        "settlements": settlements,
        "connectivity": {
            "roadAccessSettlementIds": sorted(settlement_ids),
            "railLines": [
                {"name": "Avarra Central Corridor", "settlementIds": ["settlement-001", "settlement-002", "settlement-003", "settlement-091"]},
            ],
            "waterRoutes": [
                {"name": "River-to-Inland-Sea Ferry", "mode": "passenger-and-light-freight", "settlementIds": ["settlement-001", "settlement-021", "settlement-003"]},
            ],
            "airLinks": [
                {"name": "Avarra Regional Air Loop", "settlementIds": ["settlement-001", "settlement-002", "settlement-003"]},
            ],
            "airportRoles": [
                {"settlementId": "settlement-001", "role": "international_gateway"},
                {"settlementId": "settlement-002", "role": "coastal_international_and_domestic"},
                {"settlementId": "settlement-003", "role": "regional_inland_sea_airport"},
            ],
        },
        "npcBaseline": {
            "archetypes": ["resident", "shopkeeper", "barber", "food_vendor", "commuter", "student", "teacher", "medic", "police", "fire_responder", "mechanic", "transit_operator", "road_maintenance", "sanitation_worker", "market_vendor", "farmer", "fisher", "ferry_worker"],
            "dailyRhythm": [
                {"window": "dawn", "activities": ["market_setup", "farm_and_fish_departures", "public_transport_start", "street_cleaning"]},
                {"window": "day", "activities": ["school_and_work", "shops_and_barbers_open", "civic_services", "freight_and_ferry_operations"]},
                {"window": "evening", "activities": ["return_commute", "food_stalls", "market_peak", "parks_and_waterfront"]},
                {"window": "night", "activities": ["reduced_transit", "shift_work", "emergency_services", "residential_quiet_period"]},
            ],
            "simulationRule": "Use authored schedules and bounded active-agent budgets; do not spawn one full-time NPC per resident.",
        },
        "regionSystems": ["river_and_inland_sea", "warm_fertile_crescent", "delta_flood_management", "coastal_storm_shelters", "regional_rail", "intercity_roads", "ferry_and_freight", "airport_connectivity", "water_and_sanitation", "power_and_waste", "shops_and_barber_services", "schools_and_clinics", "emergency_response"],
        "acceptanceCriteria": [
            "Every one of the 20 Avarra countries has at least one named settlement.",
            "All cities expose the city baseline, including a barber shop and daily retail services.",
            "Every village has safe water, sanitation, power access, a school/community hub, emergency access and a mobile barber service.",
            "Every settlement has an explicit road-access entry; rail, ferry and air links have valid named endpoints.",
            "Country ownership and flags are reviewable per settlement; no legacy single-country owner is used in this manifest.",
            "This design manifest is not described as a playable PC/PS5 build until a native renderer and runtime loader exist.",
        ],
    }


def main() -> None:
    manifest = build_manifest()
    out = ROOT / "data/region-baselines/avarra-crescent.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    counts = manifest["settlementCounts"]
    print(f"Wrote {out.relative_to(ROOT)}: {counts['total']} settlements across {counts['countriesCovered']} countries")


if __name__ == "__main__":
    main()
