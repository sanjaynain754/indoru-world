import json
import re
import argparse
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description='Build the canonical Indoru world registries.')
    parser.add_argument(
        '--source',
        type=Path,
        default=ROOT / 'data' / 'countries-source.md',
        help='approved country design Markdown file (repository-relative by default)',
    )
    return parser.parse_args()


args = parse_args()
source_path = args.source if args.source.is_absolute() else ROOT / args.source
if not source_path.is_file():
    raise SystemExit(f'country source not found: {source_path}')
source = source_path.read_text(encoding='utf-8')
regions = {
    'Avarra Crescent': ('AVR', 'warm fertile crescent, inland sea trade'),
    'Khoruun Reach': ('KHR', 'canyons, salt flats, plateaus and storm coast'),
    'Velmora Isles': ('VLM', 'islands, lagoons, ports and marine trade'),
    'Orsik Plateau': ('ORS', 'high altitude grasslands, mountains and cold basins'),
    'Nembasa Greenbelt': ('NEM', 'rainforest, wetlands, rivers and biodiversity'),
    'Dravik Arc': ('DRV', 'volcanic islands, geothermal zones and ash plains'),
    'Erynd Polar Ring': ('ERY', 'ice islands, research stations and polar waters'),
}
region_order = list(regions)
current_region = None
countries = []
for line in source.splitlines():
    heading = re.match(r'^## (.+?) — (\d+) countries', line)
    if heading:
        current_region = heading.group(1)
        continue
    row = re.match(r'^\|\s*(\d+)\s*\|\s*([^|]+?)\s*\|\s*([^|]+?)\s*\|', line)
    if row and current_region in regions:
        number, name, identity = int(row.group(1)), row.group(2).strip(), row.group(3).strip()
        code, theme = regions[current_region]
        status = 'playable'
        countries.append({
            'countryId': f'country-{number:03d}',
            'name': name,
            'displayName': name,
            'region': current_region,
            'regionCode': code,
            'flagId': f'flag-{name.lower()}',
            'flagAsset': f'assets/flags/flag-{name.lower()}.svg',
            'identity': identity,
            'regionalTheme': theme,
            'status': status,
            'unlockOrder': number,
            'mapKey': f'country/{name.lower()}',
            'peopleProfileId': f'people-{name.lower()}',
        })

world = {
    'worldId': 'indoru-world-001',
    'name': 'Indoru',
    'version': '0.1.0',
    'mapAsset': 'assets/maps/indoru-transport-network.png',
    'startingCountryId': next((c['countryId'] for c in countries if c['name'] == 'Avenra'), None),
    'countryCount': len(countries),
    'temporaryMapCodes': False,
    'countries': countries,
    'regions': [
        {'regionId': code.lower(), 'name': name, 'countryCount': sum(c['region'] == name for c in countries), 'theme': theme}
        for name, (code, theme) in regions.items()
    ],
    'comingSoonPolicy': {
        'visibleOnWorldMap': True,
        'playable': True,
        'unlockByExpansion': False,
        'label': 'Available Now',
        'requiresCountryNameAndFlag': True,
    }
}
if world['startingCountryId'] is None:
    raise SystemExit('approved source must contain Avenra as the starter country')
if sum(c['status'] == 'playable' for c in countries) != len(countries):
    raise SystemExit('approved source must produce playable records for every country')
if countries[0]['name'] != 'Avenra' or world['startingCountryId'] != countries[0]['countryId']:
    raise SystemExit('Avenra must remain the default startingCountryId')

(ROOT / 'data/world.json').write_text(json.dumps(world, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
(ROOT / 'data/countries.json').write_text(json.dumps(countries, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(f'Wrote {len(countries)} countries; playable={sum(c["status"] == "playable" for c in countries)}; coming_soon={sum(c["status"] == "coming_soon" for c in countries)}')
