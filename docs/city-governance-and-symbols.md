# Indoru City Governance, Administrative Laws and Symbols

## Constitutional hierarchy

Indoru एक single country है। सभी cities और villages उसी national government तथा Indoru national flag के अधीन आते हैं। National level पर **Indoru National Assembly** laws, national budget, citizenship, currency, border policy, national transport, defense और country-wide justice framework संभालती है। Regional Administration climate, roads, water, environment और inter-city planning coordinate करती है। हर city में City Council और Mayor/City Executive होगा। District Offices और Ward Councils local services चलाएँगे, जबकि villages में Village Councils होंगे।

Local government को national law के विरुद्ध अपनी अलग sovereignty, border या national flag बनाने का अधिकार नहीं होगा। Local symbols केवल municipal identity, sports, festivals, signage और civic services के लिए होंगे। हर city building, airport, police vehicle और official document में Indoru का national flag primary identity रहेगा; local symbol secondary mark होगा।

## Common administrative laws

| Law | Game-world effect |
|---|---|
| Civic Safety Code | emergency orders, evacuation, public safety और crowd control को regulate करता है |
| Due Process Charter | arrest, search, seizure और penalties को evidence, authorization और review से बाँधता है |
| Public Space Code | roads, markets, parks और transit को सुरक्षित तथा accessible रखता है |
| Transport Code | licensing, speed, vehicle safety, cargo और emergency priority तय करता है |
| Land and Water Stewardship Act | construction, mining, forest, river और coast permits regulate करता है |
| Data and Identity Protection Act | civic records, AI systems और identity data की access audit करता है |
| Local Accountability Rule | city budgets, contracts और public decisions को reviewable बनाता है |

ये laws gameplay systems में permits, inspections, public hearings, elections, fines, appeals, emergency response, service reputation और agency behavior के रूप में दिखाई देंगे। Crime या administrative action को arbitrary instant punishment नहीं बनाया जाएगा; evidence, authorized process और appeal path मौजूद रहेंगे।

## City governance models

### Navaar

Navaar capital में 48-seat Capital Civic Council और Capital Mayor होगा। National Civic Secretariat, Central Police Command, Supreme Court Office और Emergency Coordination Authority city की विशेष agencies होंगी। Capital Security and Assembly Ordinance, Riverfront Heritage Code और National Government Access Protocol स्थानीय rules होंगे। इसका symbol golden four-way compass over a blue river arc है, जो country के regions को capital से जोड़ने का अर्थ रखता है।

### Solmera

Solmera में 36-seat Coastal Metropolitan Council और Coastal Mayor होगा। Sun Coast Authority, Harbor and Tourism Board, Solar Energy Office और Coastal Rescue Service प्रमुख agencies होंगी। Beach and Lagoon Protection Code, Solar Infrastructure Safety Rule और Seasonal Harbor Closure Act लागू होंगे। इसका local symbol open sun disc और three tide curves है।

### Mirqara Prime

Mirqara Prime में 40-seat Trade Metropolitan Council और Port Mayor होगा। Inland Sea Port Authority, Customs Directorate, Trade Standards Office और Marine Health Service city की administration चलाएँगे। Fair Cargo and Customs Code, Merchant Transparency Act और Inland Sea Navigation Rule इसके प्रमुख laws होंगे। Symbol तीन linked rings और harbor star से बनेगा, जो trade routes का संकेत है।

### Khoruun City

Khoruun City में 32-seat Industrial Regional Council और Canyon Mayor होगा। Canyon Transport Authority, Mining Inspectorate, Mountain Rescue Command और Water Reserve Office प्रमुख agencies होंगी। Canyon Water Reserve Act, Quarry Safety Code और Heavy Freight Route Rule लागू होंगे। इसका symbol black mountain chevron तथा copper road line है।

### Vaskora Bay

Vaskora Bay में 38-seat Storm Coast Council और Bay Mayor होगा। Storm Authority, Flood Control Office, Coast Guard Command और Weather Observatory city governance का हिस्सा होंगे। Floodplain Building Code, Storm Warning Compliance Act और Shipyard Environmental Rule लागू होंगे। Symbol raised breakwater, wave और beacon से बनेगा।

### Velmora Central

Velmora Central में 30-seat Island Federation Council और Island Speaker होगा। Island Ferry Authority, Marine Rescue Fleet, Port Customs Office और Tide Assembly Secretariat प्रमुख agencies होंगी। Island Ferry Safety Code, Tide Access and Dock Rule और Reef Protection Act इसके local laws होंगे। Symbol पांच connected island diamonds और tidal crescent होगा।

## Normal-city governance

Normal cities में 14–18 seat local councils होंगे। उनकी responsibilities water distribution, roads, local markets, schools, clinics, waste, public safety, permits, parks और local transport तक सीमित रहेंगी। Arela forest stewardship, Brena slope and quarry safety, Cavora lake and irrigation, Dalena canals, Evaraa freight safety, Farona wildlife, Girela water distribution, Havora snow rescue, Ivaraa fishing, Jalena education, Kerona rail-farming और Luraa ferry/lighthouse systems पर focused होंगी।

Normal city symbols के motifs local geography से आएँगे—leaf and river, stepped hills, water clock, rain drops, road wheel, forest canopy, desert spring, snow peak, fish, open book, grain and rail, तथा lighthouse and tide। ये symbols original municipal marks होंगे और Indoru national flag का स्थान नहीं लेंगे।

## Flag and symbol design system

Indoru national flag primary flag रहेगा। City-level flags को rectangular civic banners की तरह design किया जाएगा, जिनमें एक unique local palette, one central motif और simple geometry होगी। किसी existing national flag, government seal, corporate logo, religious emblem या recognizable protected symbol की copy नहीं की जाएगी। City symbols को 32-pixel icon, road sign, airport badge, police patch, UI marker और large civic building facade पर readable होना चाहिए।

हर symbol metadata में `symbolId`, city reference, motif, palette, meaning, revision और asset path store होगा। Local symbol किसी city की administrative identity दिखाएगा; country ownership `country-indoru` और national flag reference अलग fields में हमेशा मौजूद रहेंगे।

## Governance gameplay

Player city council services, permits, elections, public hearings, local jobs, police complaints, transport fines, emergency alerts और business licensing के माध्यम से governance systems देख सकेगा। City budget बदलने से road maintenance, NPC service speed, hospital capacity, patrol coverage और airport operations प्रभावित हो सकते हैं। Major city agencies national law के अधीन रहेंगी, लेकिन local priorities अलग होंगी।

## Data files

Machine-readable governance और symbol data `data/city-governance.json` में है। यह data city IDs को country ID, national flag reference, governance model, council size, agencies, local laws और symbol design से जोड़ता है। सभी playable countries के governance records उनके respective country packages में जोड़े जाएँगे।
