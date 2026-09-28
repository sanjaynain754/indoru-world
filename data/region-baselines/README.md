# Region Baselines

These manifests are **engine-neutral content-design inputs**, not claims that a region is already playable or PS5-ready. They must be loaded by a native runtime and visually verified before any country/settlement is marked playable.

## Avarra Crescent v1

- 20-country roster copied from `data/countries.json`.
- 36 settlement entries: 1 capital, 2 other major cities, 13 normal cities, and 20 villages.
- Every country has at least one settlement; the last four villages extend the legacy 32-entry Avarra list to cover the final four countries.
- City and village facility baselines include barber access, health, education, utilities, emergency response, markets and transportation.
- Road access is declared for every settlement; rail, river-ferry and airport links name their endpoints.
- Ownership assignments beyond the confirmed Avenra starter context are marked **proposed** and require review before migrating into the global canonical registry.
- The manifest intentionally omits player-facing unlock labels and does not change the legacy global registry or the current browser prototype.

## Rebuild and validate

```bash
python3 scripts/build_avarra_baseline.py
python3 scripts/validate_region_baselines.py
python3 -m unittest scripts/test_avarra_baseline.py -v
```

Current repository status still lacks a native renderer/runtime loader. Passing these checks validates data consistency only; it does not produce a playable desktop or PS5 build.
