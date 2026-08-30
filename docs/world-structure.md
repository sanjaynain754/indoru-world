# Indoru World Structure

## Purpose

यह repository Indoru के original fictional world का source of truth है। कोई temporary map marker जैसे `D1`, `D2`, `E1` या `E2` game data में उपयोग नहीं होगा। हर country का स्थायी `countryId`, official `name`, `flagId`, `flagAsset`, region और gameplay status होगा।

## Data model

`data/world.json` global world registry है। इसमें world identity, region registry, starting country, map asset और Coming Soon policy रखी जाती है। `data/countries.json` सभी 120 countries की canonical list है। प्रत्येक record में country name, fictional identity, region, flag reference, map key, people profile reference और status शामिल हैं।

### Required country fields

| Field | Meaning |
|---|---|
| `countryId` | Stable internal identifier, such as `country-001` |
| `name` | Player-facing fictional country name, such as `Avenra` |
| `region` | Parent region name |
| `flagId` | Stable flag identifier, independent of image filename |
| `flagAsset` | Future SVG/PNG flag asset path |
| `identity` | Short geography/culture/economy identity |
| `status` | Either `playable` or `coming_soon` |
| `mapKey` | Stable map-loading namespace based on country name |
| `peopleProfileId` | Link to future population and AI profile |

## Status rules

पहले release में केवल **Avenra** playable starter country है। बाकी 119 countries world map पर नाम और flag preview के साथ दिखाई देंगे, लेकिन उनके buttons disabled होंगे और label **Coming Soon** होगा। कोई Coming Soon country accidentally playable नहीं बनेगी; उसे expansion unlock configuration से explicitly promote करना होगा।

Coming Soon table में प्रत्येक country का नाम, flag preview, region, short identity, unlock order और planned feature note दिखाया जा सकता है। इसका उद्देश्य players को future world समझाना है, न कि unfinished content को playable बताना।

## Flags

हर flag का `flagId` country name से stable रूप से जुड़ा रहेगा। Final assets SVG और PNG दोनों formats में बनाए जाएँगे। Flag designs original geometric language का उपयोग करेंगे और real-world national flags, logos, protected emblems या copied compositions को reuse नहीं करेंगे। Asset generation के बाद flag metadata में palette, motif, symbolism और revision number जोड़ा जाएगा।

## Map loading

Global map केवल country names और flags का display registry पढ़ेगा। Detailed 3D terrain बाद में `mapKey` के आधार पर streamed region से load होगा। उदाहरण के लिए `country/avenra` playable map package को load करेगा, जबकि `country/khorava` अभी world-map preview और Coming Soon card दिखाएगा।

## Expansion workflow

नई country को playable बनाने के लिए उसका terrain package, cities, villages, roads, weather profile, people profile, missions, flag asset, moderation rules और server configuration पूरा होना चाहिए। इसके बाद `status` को `playable` और `unlockOrder` को defined value में बदला जाएगा। इस workflow से नाम और flag पहले से मौजूद रह सकते हैं, जबकि game content सुरक्षित रूप से बाद में unlock होगा।

## Settlement registry

Indoru world में पहला settlement registry **15 major cities, 100 normal cities और 150 villages** रखता है। कुल 265 settlements में से Avenra से जुड़े settlements `playable` हैं और बाकी countries से जुड़े settlements `coming_soon` हैं। इस तरह city और village records अपने country के नाम और flag reference से जुड़े रहते हैं; temporary region codes का उपयोग नहीं होता।

Major city tier में capitals और बड़े strategic hubs आते हैं। Normal city tier में regional commerce, industry, transport और services वाले cities आते हैं। Village tier में farming, fishing, crafts, forest, hill, desert-edge और highway communities आती हैं। प्रत्येक record में stable settlement ID, country ID, display name, type, population tier, map key, country flag reference और unlock label है।

`data/settlements.json` को world-map table, country detail screen और future server-side streaming system पढ़ सकते हैं। Playable entries के लिए UI `Available Now` और entry action दिखाएगा। Unfinished entries के लिए official settlement name, country flag reference और **Coming Soon** label दिखेगा, लेकिन detailed terrain, NPC schedules और missions locked रहेंगे।
