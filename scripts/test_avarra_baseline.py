#!/usr/bin/env python3
"""Regression tests for the Avarra region baseline data contract."""
from __future__ import annotations

import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
from build_avarra_baseline import build_manifest  # noqa: E402
from validate_region_baselines import validate_manifest  # noqa: E402


class AvarraBaselineTests(unittest.TestCase):
    def setUp(self) -> None:
        self.manifest = build_manifest()

    def test_all_twenty_countries_have_settlement_coverage(self) -> None:
        self.assertEqual(self.manifest["settlementCounts"]["countriesCovered"], 20)
        self.assertEqual(validate_manifest(self.manifest), [])

    def test_missing_country_settlement_is_rejected(self) -> None:
        candidate = copy.deepcopy(self.manifest)
        candidate["settlements"] = [
            item for item in candidate["settlements"] if item["countryId"] != "country-020"
        ]
        errors = validate_manifest(candidate)
        self.assertTrue(any("countries without a named settlement" in error for error in errors))

    def test_city_without_barber_shop_is_rejected(self) -> None:
        candidate = copy.deepcopy(self.manifest)
        candidate["facilityProfiles"]["normal_city"]["additional"].remove("barber_shop")
        errors = validate_manifest(candidate)
        self.assertTrue(any("normal_city lacks city services" in error for error in errors))

    def test_unknown_transport_endpoint_is_rejected(self) -> None:
        candidate = copy.deepcopy(self.manifest)
        candidate["connectivity"]["airLinks"][0]["settlementIds"].append("missing-city")
        errors = validate_manifest(candidate)
        self.assertTrue(any("airLinks route" in error for error in errors))

    def test_draft_is_not_mislabeled_as_playable_runtime(self) -> None:
        self.assertFalse(self.manifest["runtimeIntegrated"])
        self.assertEqual(self.manifest["contentState"], "baseline-design")
        self.assertTrue(all(item["assignmentReview"] == "proposed" for item in self.manifest["settlements"]))


if __name__ == "__main__":
    unittest.main(verbosity=2)
