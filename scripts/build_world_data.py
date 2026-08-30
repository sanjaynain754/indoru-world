import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = Path('/home/ubuntu/indoru_countries_v1.md').read_text(encoding='utf-8')
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
        status = 'playable' if name == 'Avenra' else 'coming_soon'
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
            'unlockOrder': 1 if status == 'playable' else None,
            'mapKey': f'country/{name.lower()}',
            'peopleProfileId': f'people-{name.lower()}',
        })

world = {
    'worldId': 'indoru-world-001',
    'name': 'Indoru',
    'version': '0.1.0',
    'mapAsset': 'assets/maps/indoru-global-map.png',
    'startingCountryId': 'country-001',
    'countryCount': len(countries),
    'temporaryMapCodes': False,
    'countries': countries,
    'regions': [
        {'regionId': code.lower(), 'name': name, 'countryCount': sum(c['region'] == name for c in countries), 'theme': theme}
        for name, (code, theme) in regions.items()
    ],
    'comingSoonPolicy': {
        'visibleOnWorldMap': True,
        'playable': False,
        'unlockByExpansion': True,
        'label': 'Coming Soon',
        'requiresCountryNameAndFlag': True,
    }
}
(ROOT / 'data/world.json').write_text(json.dumps(world, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
(ROOT / 'data/countries.json').write_text(json.dumps(countries, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(f'Wrote {len(countries)} countries; playable={sum(c["status"] == "playable" for c in countries)}; coming_soon={sum(c["status"] == "coming_soon" for c in countries)}')
