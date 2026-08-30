# Indoru Main Map and Navaar Capital — Detailed Design

## 1. Design goal

Indoru का main map एक fictional island-country होगा, जिसकी पहचान **एक देश, अनेक climates** है। Player को एक connected world मिलेगा जिसमें capital city, normal cities, villages, forests, rainy coast, desert, highlands, lake district और islands एक ही logical geography में जुड़े होंगे। Central global map पर मौजूद दूसरे countries और Coming Soon content इस design का हिस्सा नहीं हैं; यह document केवल playable Indoru country और उसकी capital Navaar पर केंद्रित है।

## 2. Indoru country layout

Indoru को north-to-south climate gradient और west-to-east rainfall pattern के साथ design किया गया है। उत्तर में elevation बढ़ने के कारण snow mountains हैं। पूर्वी coast पर समुद्री हवाओं से rain belt बनता है। दक्षिण में mountains की rain-shadow के कारण Sura Desert है। पश्चिमी भाग में नदी, forest और low hills हैं। बीच में Navaar Capital Basin है, जहाँ देश की सबसे अधिक population और road connectivity होगी।

| Region | Position | Environment | Main settlements | Gameplay identity |
|---|---|---|---|---|
| Shivren Highlands | north | snow peaks, cold valleys | Shivren Base, hill villages | rescue, mining, mountain travel |
| Velan Forest Belt | west | dense forest, rivers, fog | forest villages, Dharvek edge | exploration, wildlife, hidden routes |
| Navaar Capital Basin | center | warm plains, river and urban land | Navaar and surrounding towns | first story, government, commerce |
| Kora Rain Coast | east | humid coast, heavy rain, cliffs | Koral Port, fishing villages | ships, storms, port economy |
| Nivor Lake District | northwest-center | lake, farms, cool air | Miren Lake | water transport, tourism, dam |
| Arak Dry Hills | southwest | scrubland, quarries, dry valleys | Dharvek corridor | farms, quarry, rural driving |
| Sura Desert | south | dunes, rocky desert, hot days | Surok Gate, desert villages | long drives, water logistics, survival |
| Mira Coast and Islands | southeast | beaches, reefs, small islands | ferry villages and resorts | boats, tourism, coastal missions |

## 3. Main travel network

Navaar के चारों ओर एक **Capital Ring Road** होगा। North-South Crown Highway capital को Shivren Highlands और Sura Desert से जोड़ेगा। West Forest Road Velan Forest और Dharvek corridor तक जाएगा। East Rain Road Koral Port तक पहुँचेगा। Lake Route Miren Lake, dam और farming settlements से जुड़ा होगा। Southern Desert Bypass heavy vehicles और long-distance traffic के लिए होगा।

देश में दो main bridges, एक mountain tunnel, एक rail tunnel, एक dam crossing, airport access road, ferry network और cargo railway होगी। Roads को केवल decorative lines की तरह नहीं रखा जाएगा; traffic, weather, police patrols, delivery missions और NPC daily commutes इन्हीं routes पर चलेंगे।

### Suggested first-release travel times

| Route | Approx. game travel time | Transport focus |
|---|---:|---|
| Navaar to Koral Port | 8–12 minutes | city road, rain coast highway |
| Navaar to Dharvek | 7–10 minutes | hills, farms, quarry roads |
| Navaar to Miren Lake | 10–14 minutes | river road, bridge, lake district |
| Navaar to Velan Forest edge | 6–9 minutes | forest road and fog |
| Navaar to Surok Gate | 14–18 minutes | desert highway and fuel stops |
| Navaar to Shivren Base | 18–24 minutes | mountain highway and tunnel |

## 4. Navaar capital city overview

Navaar Indoru की capital, largest urban center और first playable city होगी। इसका layout circular basin और river corridors पर आधारित होगा। पुराना city center नदी के eastern bank पर होगा, Government Quarter north-east में, modern commercial skyline south-east में और industrial belt south-west में। Ring Road बाहरी districts को जोड़ते हुए city traffic को central streets से अलग रखेगी।

Navaar का urban design तीन layers में होगा: **Old Core**, **Modern Ring** और **Outer Metro Belt**। Old Core में tight streets, bazaars और historic buildings होंगे। Modern Ring में offices, apartments, hospitals, universities और transport hubs होंगे। Outer Metro Belt में warehouses, worker housing, farms, airport road और highway service settlements होंगे।

## 5. Navaar districts

| District | Character | Important locations | Typical people |
|---|---|---|---|
| Old Core | historic dense center | central square, old market, archive | traders, tourists, residents, officers |
| Government Quarter | administrative center | parliament, ministries, court, civic plaza | officials, journalists, lawyers, police |
| Riverfront | public and leisure corridor | river walk, bridges, ferry pier, parks | families, vendors, boat operators |
| Navara Market Ward | crowded commercial district | bazaar, repair lanes, food streets | shopkeepers, delivery workers, visitors |
| Meridian Business Ring | modern skyline | banks, offices, hotels, convention hall | executives, staff, security, drivers |
| University Ward | education and research | university, library, student housing, labs | students, professors, researchers |
| Southworks Industrial | factories and logistics | cargo yard, warehouses, power substation | mechanics, workers, freight crews |
| Green Terrace | middle-class residential zone | apartments, schools, clinic, sports ground | families, teachers, children |
| East Gate | main entry district | airport road, bus terminal, customs gate | travelers, taxi drivers, customs staff |
| West Orchard Edge | city-rural transition | farms, water channels, small villages | farmers, gardeners, local vendors |
| Harbor Link | connection to Kora route | freight depot, ferry office, coastal highway | sailors, merchants, inspectors |
| Highview Heights | elevated residential zone | embassy-like civic residences, observatory | officials, professionals, security |

## 6. Capital landmarks

Navaar का central landmark **The Meridian Tower** होगा, जो city navigation और government skyline का visual anchor होगा। **Civic Ring** के अंदर parliament, court, public records hall और central administration campus होंगे। **Navaar Central Station** railway, bus और city transit को जोड़ने वाला major hub होगा। **River Crown Bridge** Old Core और Riverfront को जोड़ेगा। **Navaar Grand Market** commerce और social encounters का सबसे busy location होगा। **Southworks Cargo Yard** logistics, industrial missions और freight AI के लिए होगा। **Navaar General Hospital**, **Central Police Command**, **University of Indoru**, **Civic Stadium** और **East Gate Airport** city के systemic anchors होंगे।

## 7. City circulation and zones

Capital में pedestrian streets Old Core और Market Ward में अधिक होंगी। Cars और buses Modern Ring तथा Outer Metro Belt में अधिक चलेंगे। Emergency vehicles के लिए Government Quarter, hospital और police command के बीच priority corridors होंगे। Riverfront में walking, ferry और cycle routes होंगे। Industrial district में heavy trucks के समय-based restrictions होंगे। Weather के कारण rain days में low streets और underpasses temporarily slow या block हो सकते हैं।

## 8. People and gameplay systems

Navaar की population role-based और schedule-based होगी। सुबह office workers, students और delivery traffic बढ़ेंगे। दोपहर में markets और public services busy होंगे। शाम को Riverfront, stadium और food streets active होंगे। रात में industrial freight, security patrols और limited public transport दिखाई देंगे। Rain, festivals, sports events, strikes और road closures daily population patterns बदलेंगे।

Player को शुरुआती roles में citizen, driver, shop worker, mechanic, journalist, police trainee, investigator, courier और entrepreneur जैसे options मिल सकते हैं। Government, court और police systems fictional rules पर चलेंगे। Crime gameplay में evidence, witnesses, reports, pursuit, arrest, legal process और reputation शामिल होंगे; fixed five-star meter नहीं होगा।

## 9. Map content by release

पहले playable release में Navaar Old Core, Riverfront, Navara Market Ward, Green Terrace, East Gate और Southworks Industrial के selected portions खुलेंगे। Koral Port road, Velan Forest edge और Arak Dry Hills का short corridor connected exploration देंगे। Shivren Highlands, Sura Desert, Mira Islands और बाकी districts map पर visible रह सकते हैं, लेकिन detailed missions और full terrain later expansion में आएँगे।

| Version | Unlock scope |
|---|---|
| Indo 1.0 | Navaar starter districts, initial roads, basic NPC AI and first missions |
| Indo 1.1 | full Riverfront, East Gate and Koral Port route |
| Indo 1.2 | Green Terrace, University Ward and normal-city corridor |
| Indo 2.0 | Dharvek, Arak Dry Hills and Velan Forest edge |
| Indo 3.0 | Miren Lake, Sura Desert and Shivren travel routes |
| Indo 4.0 | Mira Coast, islands, advanced weather and broader multiplayer systems |

## 10. Data and server linkage

हर district, city, village, road और landmark को stable IDs दिए जाएँगे। City और village records अपने parent country `country-001` से जुड़ेंगे और country का flag reference use करेंगे। Settlement IDs केवल map loading और gameplay identity के लिए होंगे; वे अलग country नहीं माने जाएँगे। Coming Soon countries और उनके future settlements इस playable central-map package से अलग रहेंगे।

## Final design rule

Indoru का main map बड़ा दिखे, लेकिन खाली न लगे। प्रत्येक road का destination, प्रत्येक district का social purpose और प्रत्येक weather zone का geographic कारण होना चाहिए। Navaar first release का dense, readable और believable core होगा; बाकी country उसके चारों ओर gradual expansion के रूप में खुलेगी।
