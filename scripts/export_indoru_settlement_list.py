import json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
data = json.loads((root / 'data/settlements.json').read_text(encoding='utf-8'))['settlements']
major = [x for x in data if x['type'] in ('capital', 'major_city')]
normal = [x for x in data if x['type'] == 'normal_city']
villages = [x for x in data if x['type'] == 'village']

lines = [
    '# Indoru — Central Map Settlement List',
    '',
    '> यह list केवल central playable Indoru map के लिए है। सभी settlements Indoru country के अंदर आते हैं; वे अलग देश नहीं हैं और country का national flag use करेंगे। Global Coming Soon countries इस list में शामिल नहीं हैं।',
    '',
    '| Category | Count | Ownership |',
    '|---|---:|---|',
    f'| Major cities | {len(major)} | Indoru country |',
    f'| Normal cities | {len(normal)} | Indoru country |',
    f'| Villages | {len(villages)} | Indoru country |',
    f'| **Total** | **{len(data)}** | **Indoru country** |',
    '',
    '## Naming and hierarchy',
    '',
    'हर settlement का अपना नाम और settlement ID है, लेकिन उसका `countryId` Indoru country से जुड़ा है। Cities और villages administrative तथा gameplay units हैं; वे sovereign countries नहीं हैं। सभी settlements Indoru का flag use करेंगे।',
    '',
    '## 1. Major cities — 6',
    ''
]
for i, x in enumerate(major, 1):
    lines.append(f'{i}. **{x["name"]}** — {x["description"]}; `{x["unlockLabel"]}`')
lines += ['', '## 2. Normal cities — 85', '']
for i, x in enumerate(normal, 1):
    lines.append(f'{i}. **{x["name"]}** — {x["description"]}; `{x["unlockLabel"]}`')
lines += ['', '## 3. Villages — 110', '']
for i, x in enumerate(villages, 1):
    lines.append(f'{i}. **{x["name"]}** — {x["description"]}; `{x["unlockLabel"]}`')
lines += ['', '## Data fields', '', '`settlementId`, `name`, `countryId`, `region`, `type`, `status`, `flagRef`, `mapKey`, `populationTier` और `unlockLabel` प्रत्येक record में मौजूद हैं। `Coming Soon` content को इस central list में modify नहीं किया गया है।', '']
(root / 'docs/indoru-settlement-list.md').write_text('\n'.join(lines), encoding='utf-8')
print(f'Exported {len(major)} major cities, {len(normal)} normal cities and {len(villages)} villages')
