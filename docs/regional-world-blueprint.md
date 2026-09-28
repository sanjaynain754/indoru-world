# Indoru World — सातों Regions का Playable Baseline Blueprint

**दर्जा:** Design blueprint v1 — यह अभी game-code या world-data implementation नहीं है।
**लक्ष्य:** केवल high-end **desktop gaming PC और PS5** के लिए एक बड़ा, playable Indoru world—mobile या laptop release target नहीं। सातों regions क्रम से बनें; हर region में उसके शहर, कस्बे, गाँव, जरूरी सुविधाएँ, transport और NPC जीवन baseline स्तर पर हों। बाद के चरण में हर country की अलग पहचान और अधिक detail जोड़ी जाए।

## 1. लक्ष्य और निर्णय

- **Indoru world/app का नाम है; Avenra playable country है।** `data/world.json` में Avenra (`country-001`) Avarra Crescent का central-river starter है।
- आपने “payable” कहा है; इस blueprint में उसे **playable** समझा गया है—किसी region या country को खरीदकर unlock कराने का design नहीं है। Final game में सभी सात regions और 120 countries playable होंगे। Player-facing **“Coming Soon”** labels/locked-country promises नहीं होंगे। Development के दौरान अधूरा content playable बताने के बजाय उसे review build में expose नहीं किया जाएगा।
- Build order canonical रहेगा: **Avarra Crescent → Khoruun Reach → Velmora Isles → Orsik Plateau → Nembasa Greenbelt → Dravik Arc → Erynd Polar Ring**।
- “Region complete” का अर्थ केवल एक सुंदर capital नहीं: उस region के सभी canonical country/settlement records को usable map, city/village baseline, facilities, transport और basic NPC activity मिलनी चाहिए। उसके बाद अगले region पर जाएँ।
- पहले साझा baseline; बाद में country-by-country bespoke architecture, culture, economy, interiors, story और quests। Real places से **urban patterns और infrastructure lessons** लिए जाएँगे; fictional map और buildings की अपनी identity रहेगी।

## 2. Repository की वर्तमान स्थिति — scope को सही समझना जरूरी

`data/world.json` में **7 regions और 120 countries** दर्ज हैं: Avarra Crescent (20), Khoruun Reach (18), Velmora Isles (16), Orsik Plateau (17), Nembasa Greenbelt (18), Dravik Arc (16), Erynd Polar Ring (15)। अभी केवल Avenra `playable` है। `docs/world-structure.md` अन्य 119 countries को “Coming Soon” रखने का पुराना नियम बताता है; final all-playable लक्ष्य के लिए उसे बाद के implementation phase में बदलना होगा।

`docs/world-structure.md` में मौजूदा settlement registry **6 major cities + 85 normal cities + 110 villages = 201 settlements** बताती है, लेकिन इसे central starter map का registry कहा गया है और global future settlements अलग/frozen बताए गए हैं। इसलिए **201 को सातों regions की पूरी city/village coverage नहीं मानना चाहिए**। Region implementation से पहले हर settlement को country और region से जोड़कर missing global roster निकालना होगा; फिर उसी canonical roster को game में पूरा करना होगा।

एक महत्वपूर्ण mapping conflict भी है: `data/major-cities.json` अभी सभी छह major cities को `country-indoru` / “Indoru” से जोड़ता है, जबकि canonical world registry में Indoru world है और Avenra starter country। उसी file में Navaar capital, Solmera coast, Mirqara Prime inland-sea port, Khoruun City canyon hub, Vaskora Bay storm coast और Velmora Central island hub हैं। **इन city records को Avenra या किसी अन्य country में अनुमान से bulk-assign नहीं करना है**—पहले countryId और region-to-settlement mapping को स्पष्ट, canonical source-of-truth बनाना है।

Repository में एक अलग platform gap है: मौजूदा `game/` एक browser/Vite/Babylon.js prototype है, PS5-certified build नहीं। यह blueprint world-content तय करता है; console-grade engine/platform migration, controller-first UI, streaming और performance budgets अलग engineering phase होंगे। Mobile-first/touch controls इस target का हिस्सा नहीं हैं।

## 3. सात-region roster और build order

| क्रम | Region | Countries | भूगोल/मुख्य पहचान | पहले build होने वाला settlement pattern |
|---|---|---:|---|---|
| 1 | **Avarra Crescent** | 20 | उपजाऊ नदी-घाटी, inland-sea trade, गर्म शहर | नदी वाली capital, बाजार/rail town, farm-and-river villages |
| 2 | **Khoruun Reach** | 18 | canyon, dry plateau, salt flats, storm-facing coast | canyon-gateway city, oasis/market town, दूरस्थ road-service villages |
| 3 | **Velmora Isles** | 16 | द्वीप-समूह, गहरे channels, harbours | ferry-connected island city, port town, छोटे ferry villages |
| 4 | **Orsik Plateau** | 17 | ऊँचा ठंडा plateau, पर्वतीय घाटियाँ | high-altitude hub, valley market town, water-managed villages |
| 5 | **Nembasa Greenbelt** | 18 | rainforest, नदी-जाल, wetlands और mangroves | raised river city, boat-market town, flood-resilient villages |
| 6 | **Dravik Arc** | 16 | volcanic islands, geothermal ground, rugged coast | geothermal city, port/tourism town, hazard-aware villages |
| 7 | **Erynd Polar Ring** | 15 | Arctic coast, permafrost, लंबी सर्दी/अंधकार | compact polar service hub, port/research town, remote settlements |

## 4. हर city, town और village का न्यूनतम standard

### हर शहर में

हर canonical city में, उसके आकार के अनुसार, कम-से-कम:

- **अलग-अलग colonies/neighbourhoods:** पुराना/स्थानीय केंद्र, सामान्य residential colony, बाजार/दुकानों की सड़क, civic/public-service area और जरूरत होने पर logistics/industry edge। हर शहर को एक जैसा high-rise downtown नहीं बनाना।
- **दैनिक सुविधाएँ:** grocery/food market, भोजन की जगह, **barber/hair-cutting shop**, pharmacy, clinic; बड़े शहर में hospital; school; police और fire/rescue पहुँच; mechanic/repair और fuel/charging; bank/post/communications point; public transport stop; public toilet, पार्क/खुला meeting place।
- **चलती infrastructure:** पीने का पानी, drainage/sewerage, बिजली, street lighting, waste collection, safe pedestrian routes, local streets और मुख्य route तक पहुंच। भूगोल के मुताबिक flood protection, snow clearance, slope drainage, salt-resistant materials या geothermal utility जोड़ी जाए।
- **खेल में उपयोग:** कम-से-कम एक दुकान/सेवा में NPC और player interaction; transit stop पर route; clinic/rescue में visible service loop। केवल façade/खाली इमारत को “facility built” नहीं माना जाएगा।

### हर गाँव और छोटे settlement में

घर, स्थानीय livelihood/work area, पानी/ऊर्जा, community point, छोटी दुकान/market day, सुरक्षित road/path/boat connection, और clinic/school/emergency service तक स्पष्ट पहुँच हो। हर गाँव में पूरा hospital/airport बनाना जरूरी नहीं—छोटे settlements साझा service hub या scheduled outreach से जुड़ेंगे। गाँव के market hub में barber service हो; छोटे hamlets को निकटतम town तक route मिले।

### हर region में NPC baseline

कम-से-कम residents, shopkeepers, students, service workers, commuters/drivers और public-safety staff के दैनिक routine हों। NPC घर/बस्ती → काम/स्कूल → बाजार/सेवा → घर जैसे simple schedule चलाएँ; weather और दिन/रात से route या activity बदले। Region-specific professions नीचे दिए गए हैं। इस first pass में छोटा, believable population loop पर्याप्त है—हर व्यक्ति की अलग complex AI नहीं।

## 5. Region-by-region blueprint

### 1) Avarra Crescent — पहले पूरा किया जाने वाला region

**Real-world patterns:** Ahmedabad का Sabarmati riverfront—नदी से शहर का पुनः जुड़ाव, promenade/parks, neighbourhood public realm और पर्यावरण-सुधार के लक्ष्य; Kochi का waterways/island-based urban mobility model। [1][2]

**शहर और बस्तियाँ:** Avenra में नदी-किनारे civic/market capital district; एक secondary trading/rail town; खेती, नदी-मछली और छोटे craft से चलने वाले village clusters। Existing city roster का **Navaar** river-capital role Avenra के साथ संभावित रूप से मेल खाता है, पर `countryId` mapping ठीक होने तक इसे final assignment न मानें।

**Infrastructure/connectivity:** river bridges और floodplain public paths; city bus और regional passenger/freight rail; inland-water ferry वहाँ जहाँ नक्शे में navigable water हो; capital airport और river/cargo terminal; farm-to-market secondary roads। Waterfront को usable public space बनाएं—सिर्फ concrete embankment नहीं।

**Colonies/facilities:** compact old bazaar, riverside mixed-use blocks, family residential colonies, station/market quarter, civic district, edge पर light logistics/food processing; हर city में ऊपर वाली universal facilities, barber shop समेत।

**NPC/gameplay loop:** किसान/उत्पादक → बाजार व्यापारी → rail/ferry commuter; barber और shop-closing hours; ferry/bridge पर rush; बारिश में flood-control crews। Player delivery, market run, commuter pickup और river-rescue जैसे छोटे tasks कर सके।

### 2) Khoruun Reach

**Real-world patterns:** Moab का official general plan growth और quality-of-life balance के लिए community working document है; Rajasthan heritage-town framework historic town को monument नहीं बल्कि services, mobility, livelihood और community सहित पूरे urban ecosystem की तरह देखने को कहता है। [3][4]

**शहर और बस्तियाँ:** canyon-mouth freight city; पानी/बाजार के आसपास compact oasis town; salt-flat, quarry और plateau के छोटे settlements; storm coast पर अलग port town केवल उन्हीं country maps में जहाँ तट मौजूद हो। Existing **Khoruun City** का canyon-logistics role यहां का साफ शुरुआती reference है, लेकिन country mapping अभी verify होनी है।

**Infrastructure/connectivity:** canyon के अनुरूप सीमित, readable road corridors; passing bays, tunnels/bridges केवल भूगोल उचित हो तो; quarry/mining freight siding या rail; बड़े hub पर airport/airstrip; दूर के गाँवों के लिए all-weather road और water/repair depots। Salt-flat को अनावश्यक urban sprawl से खाली रखें।

**Colonies/facilities:** historic stone/courtyard core, ordinary family colony, truck/repair yard और industrial edge को अलग रखें; shaded market, clinic/trauma access, rescue/fire, mechanic, water kiosk, barber और school/community hall।

**NPC/gameplay loop:** truckers, quarry/engineering crews, market vendors, water-maintenance staff और highway responders। Dust storm या heat में travel/repair decisions बदलें; canyon road closure पर detour mission मिले।

### 3) Velmora Isles

**Real-world patterns:** Kochi Water Metro waterways से शहर के हिस्से जोड़ती है; official page पर 15 routes और 75+ km network listed है। Stockholm archipelago की Waxholmsbolaget ferries साल भर द्वीपों और mainland harbours के बीच public transport चलाती हैं। [2][5]

**शहर और बस्तियाँ:** कई islands पर फैला compact island-federation city; एक shipyard/market port town; छोटे inhabited-island villages जिनमें dock, shelter और shared services हों। Existing **Velmora Central** ferry-and-island governance archetype है।

**Infrastructure/connectivity:** नियमित passenger ferry/water taxi + cargo boat; harbours पर waiting shelter/ticketing/loading; बड़े inhabited islands के बीच bridges तभी जब channel/terrain सही हो; main island पर local bus/rail; regional airport बड़े hub पर। Timetable, missed connection, storm cancellation और freight transfer gameplay का भाग हों।

**Colonies/facilities:** harbour-front commerce, पुराने fishing quarters, elevated/wind-safe housing, working port/shipyard और quieter residential island। हर urban hub में barber, groceries, pharmacy, clinic, school और port-rescue; हर village का dock उसकी “main street” है।

**NPC/gameplay loop:** fishers, ferry crew, dock worker, island student और supply trader। Player passengers/cargo को island-to-island ले जाए; मौसम से ferry timetable बदल सकता है।

### 4) Orsik Plateau

**Real-world patterns:** Ladakh Housing & Urban Development Department urban baseline में safe water, sanitation, solid waste, stormwater drainage, roads, street lighting, public transport और affordable housing गिनाता है। NITI Aayog की Ladakh strategy में glacier melt ponds, gravity-fed *yuras* और village-maintained irrigation channels दर्ज हैं। [6][7]

**शहर और बस्तियाँ:** high-altitude service capital; घाटी के junction पर market town; खेती/पशुपालन और seasonal water governance वाले गाँव। Leh को landscape/settlement reference मानें; player-visible names फिर canonical Indoru roster से ही आएँ।

**Infrastructure/connectivity:** plateau roads और pass routes; winter/weather closures का विकल्प; inter-city bus; feasible corridor में freight/passenger rail—हर mountain route पर rail थोपना नहीं; airport/airfield मुख्य hub पर; gravity-fed water, storage, leak control और community-run irrigation channels।

**Colonies/facilities:** wind-protected compact blocks, traditional-material old quarter, school/clinic zone, market/repair yard और carefully sited new residential colony। barber, pharmacy, clinic, warm public shelter, snow-ready fire/rescue और reliable water point अनिवार्य।

**NPC/gameplay loop:** water steward, farmer/herder, bus/road crew, winter-clinic staff, bazaar shopkeeper। Snowmelt/winter water schedule और pass closure NPC route/activity को बदलें।

### 5) Nembasa Greenbelt

**Real-world patterns:** UNESCO Sundarbans को ज्वारीय waterways, mudflats और छोटे mangrove islands के जाल के रूप में वर्णित करता है; आसपास के गाँवों की fishing/honey-gathering livelihoods और cyclone/tidal risks भी दर्ज हैं। World Bank synthesis local climate vulnerability के साथ location-specific resilience investments पर जोर देती है। [8][9]

**शहर और बस्तियाँ:** raised river-bank city; boat-to-market town; forest/wetland edge पर छोटे fishing, farming और ranger villages। Mangrove/wetland core को protected no-build zone रखें—हर सुंदर जगह पर suburb या road न डालें।

**Infrastructure/connectivity:** boats/water taxis प्राथमिक; raised footpaths, short causeways/bridges, landing stages, flood-safe evacuation shelters; जरूरी corridors में all-weather road; large airport केवल regional hub के पास, छोटे द्वीपों में नहीं। Flood marker, tide/weather warning और emergency boat response gameplay में दिखें।

**Colonies/facilities:** raised homes, compact service nodes, market/dock, clinic/school, clean-water point और community shelter; city में barber और सभी universal services। Waste/water treatment को नदी में सीधे छोड़ने वाला shortcut न बनाएं।

**NPC/gameplay loop:** fishers, boat pilots, market sellers, honey/forest livelihood NPCs, conservation rangers और cyclone-response crews। High tide, storm warning या blocked jetty daily movement और rescue mission बदलें।

### 6) Dravik Arc

**Real-world patterns:** Iceland National Energy Authority के अनुसार 2020 में domestic heating energy का लगभग 90% geothermal से आया; agency scattered settlements में छोटे geothermal sources भी बताती है। OECD की Azores study volcanic steep terrain, erosion और environmentally sensitive land के कारण compact, carefully planned settlement की जरूरत समझाती है। [10][11]

**शहर और बस्तियाँ:** geothermal-served capital; sheltered harbour/visitor town; अलग-अलग islands पर छोटे farming/fishing/geothermal villages। Reykjavík और Azores settlement pattern inspiration हैं—किसी वास्तविक city का नक्शा सीधे copy नहीं।

**Infrastructure/connectivity:** geothermal district heating केवल resource-safe zones में; grid/backup power; coastal road, regional airport और inter-island ferry जहाँ geography माँगे; evacuation roads, siren/warning network, observatory और clearly marked hazard boundaries।

**Colonies/facilities:** heated civic core, low-rise warm residential colonies, port/workshop, geothermal utility zone और protected lava/landscape buffers। Barber, groceries, clinic, school, emergency/fire-rescue, repair shop और public warm shelter।

**NPC/gameplay loop:** utility engineers, port crew, fishers, visitor-service worker, hazard monitor और emergency teams। Weather/ground alerts पर route closures और evacuation drills; रोज़मर्रा life सिर्फ disaster नहीं—markets, hot-water/public-bath social spaces और commute भी।

### 7) Erynd Polar Ring

**Real-world patterns:** Longyearbyen local council का land-use plan housing, business, outdoor recreation, technical infrastructure, conservation और transport के लिए अलग land use तय करने का ढाँचा देता है। [12]

**शहर और बस्तियाँ:** एक compact warm/insulated polar service hub; port/airport logistics और research-linked town; बहुत छोटे remote settlements जिनके लिए seasonal supply plan और emergency access पहले तय हो। Longyearbyen जैसे Arctic settlements केवल climate/urban form reference हैं।

**Infrastructure/connectivity:** airport और sea-freight harbour primary lifelines; insulated utilities, protected pipes, snow/wind-cleared priority routes, warm emergency shelter; rail/long highways तभी जहाँ भूगोल, दूरी और upkeep उचित ठहराएँ। Critical supplies, medevac और weather-delay alternate plan world simulation में हो।

**Colonies/facilities:** compact walkable blocks, enclosed heated public connections जहाँ उपयुक्त हों, service/repair quarter, port/airport work housing और conservation buffer। barber, grocery, pharmacy, clinic, school/community service, emergency response और reliable communications को मुख्य hub में रखें; छोटे outposts shared/remote services से जुड़ें।

**NPC/gameplay loop:** airport/port crew, researchers, maintenance staff, shop workers, medic/rescue team और seasonal residents। Polar day/night, blizzard और supply arrival NPC schedules तथा routes बदलें।

## 6. हर region के लिए “complete” acceptance gate

अगले region पर जाने से पहले ये सभी checks पास हों:

1. **World coverage:** canonical region/country roster से कोई country या settlement छूटा नहीं; हर record का stable countryId, display name और region association है।
2. **Playable route:** player hub तक पहुँच सकता है, city/town/village के बीच कम-से-कम एक geography-appropriate public/road route चलता है, और transit/route information समझ आती है।
3. **City life:** every city में colonies, market, barber, daily services, public space और working utilities का baseline मौजूद है।
4. **Village life:** हर canonical village/settlement पर livelihood, homes, community node और nearest shared services तक usable connection है।
5. **NPC loop:** representative NPC day/night routine निभाते हैं; region-specific job roles और weather response नजर आते हैं।
6. **Infrastructure gameplay:** airport/rail/road/ferry/water systems केवल props नहीं—कम-से-कम यात्री, cargo, emergency या commute loop में उपयोग होते हैं। अनुपयुक्त mode को जबरन न जोड़ें।
7. **Performance/content:** target PC/PS5 build में streaming, crowd/traffic budgets, save/load और controller navigation review हों; console performance targets implementation spec में तय हों।
8. **No false launch claims:** shipped world में सभी seven regions/countries playable; कोई player-facing “Coming Soon” या paid unlock gate नहीं।

## 7. पहले implementation slice की सिफारिश

**Avarra Crescent को पहले region के रूप में build करें।** लेकिन 20-country region को हाथ से अलग-अलग बनाने से पहले एक ही playable vertical slice में यह साबित करें: **Navaar/Avenra capital district + एक secondary market/rail town + दो contrasting villages** (river/farm और inland-water/transport)। इसमें market/barber, clinic, school, police/fire, homes/colonies, road/rail/river route और basic NPC schedules चालू हों। उसके बाद वही validated asset/service kit Avarra की शेष canonical settlements पर लागू करके Avarra acceptance gate पूरा करें; फिर क्रम से Khoruun शुरू करें।

> यह slice region order नहीं बदलता और scope को छोटा नहीं करता—यह पहले repeatable city/village kit को साबित करता है ताकि आगे के 120-country world में वही quality दोबारा उपयोग हो सके।

## Sources and reference material

1. [Sabarmati Riverfront Development Corporation — project objectives and city riverfront](https://www.sabarmatiriverfront.com/)
2. [Kochi Water Metro — network and waterways-based city connectivity](https://watermetro.co.in/)
3. [Moab City — General Plan](https://www.moabcity.gov/286/General-Plan)
4. [World Bank / National Institute of Urban Affairs — Rajasthan historic towns strategic framework](https://documents1.worldbank.org/curated/en/179761563443651430/pdf/Inclusive-Revitalisation-of-Historic-Towns-and-Cities-Strategic-Framework-for-Rajasthan-State-Heritage-Programme.pdf)
5. [Waxholmsbolaget — public transport by water in Stockholm archipelago](https://waxholmsbolaget.se/in-english)
6. [Housing & Urban Development Department, UT Ladakh](https://ladakh.gov.in/housing-and-urban-development/)
7. [NITI Aayog — Carbon Neutral and Climate Resilient Ladakh](https://niti.gov.in/node/684)
8. [UNESCO World Heritage Centre — The Sundarbans](https://whc.unesco.org/en/list/798/)
9. [World Bank — Coping with Climate Change in the Sundarbans](https://openknowledge.worldbank.org/entities/publication/a60b5670-cdf2-5afc-b68d-c4771e299d7d)
10. [Iceland National Energy Authority — District Heating](https://orkustofnun.is/en/natural_resources/district_heating)
11. [OECD — Adapting land-use and spatial planning to demographic change in the Azores](https://www.oecd.org/en/publications/preparing-for-demographic-change-in-the-azores-portugal_bf170c57-en/full-report/adapting-land-use-and-spatial-planning-to-demographic-change-in-the-azores_3c533e92.html)
12. [Longyearbyen Local Council — land-use plan](https://www.lokalstyre.no/engelsk/longyearbyen-local-council/news-archive/news-front-page/2026-08-17-submit-your-comments-on-the-land-use-plan)
