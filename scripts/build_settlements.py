import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CENTRAL_COUNTRY_ID = 'country-indoru'
countries = json.loads((ROOT / 'data/countries.json').read_text(encoding='utf-8'))
by_region = {}
for c in countries:
    by_region.setdefault(c['region'], []).append(c)

major = [
 ('Navaar','country-001','Avarra Crescent','capital','playable','primary government capital and starting city'),
 ('Solmera','country-002','Avarra Crescent','major_city','coming_soon','coastal finance and media metropolis'),
 ('Mirqara Prime','country-003','Avarra Crescent','major_city','coming_soon','inland-sea trade megacity'),
 ('Khoruun City','country-021','Khoruun Reach','major_city','coming_soon','canyon transport and industry center'),
 ('Vaskora Bay','country-025','Khoruun Reach','major_city','coming_soon','storm-coast shipbuilding metropolis'),
 ('Velmora Central','country-039','Velmora Isles','major_city','coming_soon','archipelago trade capital'),
 ('Lunessa','country-041','Velmora Isles','major_city','coming_soon','night-market cultural metropolis'),
 ('Orsik Heights','country-055','Orsik Plateau','major_city','coming_soon','high-altitude aviation and administration hub'),
 ('Karmuun','country-056','Orsik Plateau','major_city','coming_soon','mountain capital and engineering center'),
 ('Nembasa City','country-072','Nembasa Greenbelt','major_city','coming_soon','rainforest river megacity'),
 ('Kalemba Delta','country-074','Nembasa Greenbelt','major_city','coming_soon','delta logistics and waterway center'),
 ('Dravik Port','country-090','Dravik Arc','major_city','coming_soon','volcanic arc maritime capital'),
 ('Ashkara','country-091','Dravik Arc','major_city','coming_soon','geothermal energy metropolis'),
 ('Erynd Station','country-106','Erynd Polar Ring','major_city','coming_soon','polar research and supply capital'),
 ('Norriku','country-108','Erynd Polar Ring','major_city','coming_soon','observatory and northern-lights city'),
]

city_roots = ['Arel','Bren','Cavor','Dalen','Evara','Faron','Girel','Havor','Ivara','Jalen','Keron','Lura','Maren','Navor','Orel','Paven','Qorin','Ravel','Soren','Tavra','Ulen','Varek','Wera','Xarin','Yora','Zavel']
city_suffixes = ['a','en','is','or','um','ara','ev','on','el','ira']
regions = list(by_region)
normal = []
for i in range(100):
    region = regions[i % len(regions)]
    country = by_region[region][(i // len(regions)) % len(by_region[region])]
    name = f'{city_roots[i % len(city_roots)]}{city_suffixes[(i // len(city_roots)) % len(city_suffixes)]}'
    normal.append((name, country['countryId'], region, 'normal_city', 'playable' if country['countryId']=='country-001' else 'coming_soon', f'normal city {i+1}'))

village_roots = ['Asha','Bela','Cira','Duma','Esha','Fela','Garo','Hima','Ira','Jora','Kavi','Luma','Mira','Nala','Osha','Pira','Quma','Risa','Sava','Tira','Usha','Vina','Wala','Xira','Yuma','Zira']
village_suffixes = ['gaon','pur','vale','nadi','khet','bari','gram','toli','wadi','para','seth','kund','van','dhara','tal']
villages = []
for i in range(150):
    region = regions[(i * 3) % len(regions)]
    country = by_region[region][(i // len(regions)) % len(by_region[region])]
    name = f'{village_roots[i % len(village_roots)]}{village_suffixes[(i // len(village_roots)) % len(village_suffixes)]}'
    villages.append((name, country['countryId'], region, 'village', 'playable' if country['countryId']=='country-001' else 'coming_soon', f'village {i+1}'))

# Central playable map scope. Global Coming Soon settlements remain in the country registry and are not touched here.
major = major[:6]
normal = normal[:85]
villages = villages[:110]

records = []
for idx, row in enumerate(major + normal + villages, 1):
    name, _source_country_id, region, kind, _source_status, note = row
    # Every settlement in this file belongs to the single central country: Indoru.
    country_id = CENTRAL_COUNTRY_ID
    status = 'playable'
    records.append({
        'settlementId': f'settlement-{idx:03d}',
        'name': name,
        'countryId': country_id,
        'region': region,
        'type': kind,
        'status': status,
        'flagRef': f'country:{CENTRAL_COUNTRY_ID}',
        'mapKey': f'{country_id}/{name.lower().replace(" ", "-")}',
        'populationTier': 'mega' if kind == 'capital' else ('large' if kind == 'major_city' else ('medium' if kind == 'normal_city' else 'small')),
        'description': note,
        'unlockLabel': 'Available Now' if status == 'playable' else 'Coming Soon'
    })

payload = {
    'worldId': 'indoru-world-001',
    'version': '0.1.0',
    'counts': {'majorCities': len(major), 'normalCities': len(normal), 'villages': len(villages), 'totalSettlements': len(records)},
    'settlements': records,
    'rules': {'temporaryCodesAllowed': False, 'requiredCountryName': True, 'requiredCountryFlagRef': True, 'comingSoonVisible': True}
}
(ROOT / 'data/settlements.json').write_text(json.dumps(payload, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
print(f'Wrote {len(major)} major cities, {len(normal)} normal cities, {len(villages)} villages; total={len(records)}')
