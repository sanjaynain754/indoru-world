# Indoru World Structure

## Purpose

यह repository Indoru के original fictional world का source of truth है। कोई temporary map marker जैसे `D1`, `D2`, `E1` या `E2` game data में उपयोग नहीं होगा। हर country का स्थायी `countryId`, official `name`, `flagId`, `flagAsset`, region और gameplay status होगा।

## Data model

`data/world.json` global world registry है। इसमें world identity, region registry, default starting country, map asset और all-playable availability policy रखी जाती है। `data/countries.json` सभी 120 countries की canonical list है। प्रत्येक record में country name, fictional identity, region, flag reference, map key, people profile reference और status शामिल हैं।

### Required country fields

| Field | Meaning |
|---|---|
| `countryId` | Stable internal identifier, such as `country-001` |
| `name` | Player-facing fictional country name, such as `Avenra` |
| `region` | Parent region name |
| `flagId` | Stable flag identifier, independent of image filename |
| `flagAsset` | Future SVG/PNG flag asset path |
| `identity` | Short geography/culture/economy identity |
| `status` | Always `playable` in the current release |
| `mapKey` | Stable map-loading namespace based on country name |
| `peopleProfileId` | Link to future population and AI profile |

## Status rules

पहले release में सभी **120 countries playable** हैं। **Avenra** default starting country बनी रहती है, जबकि बाकी countries भी world map से सीधे enter की जा सकती हैं। `unlockOrder` केवल content ordering और rollout metadata है; यह किसी country को disabled या Coming Soon नहीं बनाता।

World table में प्रत्येक country का नाम, flag preview, region, short identity और unlock order दिखाया जाएगा। हर country के लिए playable action enabled रहेगा।

HUD का world-map view सभी 120 countries का keyboard- और pointer-selectable grid दिखाता है। Country चुनने पर region, identity और `mapKey` summary दिखाई जाती है; `ENTER COUNTRY` action selected map package को अगली runtime loading integration के लिए तैयार मानता है।

हर country package में deterministic seed से generated geography layer है: elevation और mountain landmarks, एक या अधिक rivers, lakes, capital/villages, connected roads, river bridges, intercity rail, optional airports/ports/military bases, और metro/airline/ship/submarine route contracts। यह procedural foundation runtime में Babylon meshes के रूप में render होती है; production-quality authored assets बाद में इन्हीं stable IDs को replace कर सकते हैं।

## Flags

हर flag का `flagId` country name से stable रूप से जुड़ा रहेगा। Final assets SVG और PNG दोनों formats में बनाए जाएँगे। Flag designs original geometric language का उपयोग करेंगे और real-world national flags, logos, protected emblems या copied compositions को reuse नहीं करेंगे। Asset generation के बाद flag metadata में palette, motif, symbolism और revision number जोड़ा जाएगा।

## Map loading

Global map केवल country names और flags का display registry पढ़ेगा। हर country का detailed 3D terrain `mapKey` के आधार पर playable streamed region से load होगा। उदाहरण के लिए `country/avenra` और `country/khorava` दोनों playable map packages load कर सकते हैं।

## Expansion workflow

हर country को playable बनाने के लिए उसका terrain package, cities, villages, roads, weather profile, people profile, missions, flag asset, moderation rules और server configuration उपलब्ध होना चाहिए। `status` सभी records में `playable` रहेगा; `unlockOrder` केवल deterministic content ordering के लिए है।

## Settlement registry

Indoru के central playable map में settlement registry **6 major cities, 85 normal cities और 110 villages** रखता है। कुल 201 settlements playable हैं। हर country के settlements भी इसी all-playable model में अपने country name और flag reference से जुड़े रहेंगे; temporary region codes का उपयोग नहीं होता।

Major city tier में 6 capitals और बड़े strategic hubs आते हैं। Normal city tier में 85 independent regional commerce, industry, transport और services वाले cities आते हैं; वे किसी major city या village के अंदर nested नहीं हैं। Village tier में 110 independent farming, fishing, crafts, forest, hill, desert-edge और highway communities आती हैं। प्रत्येक record में stable settlement ID, country ID, display name, type, population tier, map key, country flag reference और unlock label है।

`data/settlements.json` को world-map table, country detail screen और future server-side streaming system पढ़ सकते हैं। हर entry के लिए official settlement name, country flag reference और **Available Now** action दिखेगा।
