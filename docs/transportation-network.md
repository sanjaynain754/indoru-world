# Indoru Roads, Railways and Major Transportation Network

## Overview

Indoru का transportation network छह major cities, normal cities, villages और अलग-अलग climate regions को जोड़ने के लिए बनाया गया है। Network का उद्देश्य केवल travel आसान करना नहीं, बल्कि traffic, cargo, NPC routines, airports, weather disruptions, emergency response और future expansion को gameplay का हिस्सा बनाना है।

सभी routes Indoru country के अंदर हैं। Cities और villages country की territory में connected settlements हैं; वे अलग countries नहीं हैं। सभी national transport offices, airports और public vehicles Indoru का national flag use करेंगे।

## Network hierarchy

| Layer | Infrastructure | Purpose |
|---|---|---|
| Tier 1 | six-city expressways, Capital Ring Road, intercity rail और major airports | fast national travel और strategic logistics |
| Tier 2 | regional highways, secondary rail, ports और ferry chains | cities, towns, lakes और coastal regions को जोड़ना |
| Tier 3 | local roads, village roads, farm tracks, mountain passes और desert tracks | last-mile access और exploration |
| Special | tunnels, bridges, dams, emergency corridors और temporary detours | terrain crossing और dynamic events |

## Primary road network

### Capital Ring Road

Navaar के चारों ओर छह-lane Capital Ring Road होगा। यह city center का traffic bypass करेगा और airport, cargo yard, Government Quarter, Harbor Link तथा outer residential districts को जोड़ेगा। Ring के चार बड़े interchanges होंगे। Emergency vehicles के लिए dedicated priority lanes और heavy trucks के लिए time windows रखे जाएँगे।

### Crown Highway

Crown Highway Navaar से north की ओर Shivren Highlands और Shivren Base तक जाएगी। शुरुआत में चार lanes की urban expressway होगी, फिर foothills में दो-lane climbing route और अंत में mountain tunnel तथा snow pass में बदलेगी। Snow, ice और rockfall के कारण route closures तथा rescue missions possible होंगे।

### East Rain Expressway

यह Navaar को Kora Rain Coast और Koral Port से जोड़ेगी। Coastal section पर four-lane divided highway, raised bridges और flood channels होंगे। Heavy rain के दौरान low connectors बंद हो सकते हैं, और traffic AI alternate inland route चुनेगी। Koral Port से Vaskora Bay तक coastal freight और ferry transfer इसी corridor से जुड़ेंगे।

### Southern Desert Bypass

यह राजधानी से Sura Desert और Surok Gate तक जाएगी। Route के साथ fuel stations, water depots, repair yards, shaded rest stops और emergency beacons होंगे। Dust storms में visibility घटेगी, NPC convoys speed कम करेंगे और navigation safer detour सुझाएगा।

### West Forest Route

West Forest Route Navaar को Velan Forest edge और Dharvek corridor से जोड़ेगी। यह winding two-lane road होगी जिसमें river bridges, fog sections, ranger gates और fallen-tree event zones होंगे। Forest route पर heavy traffic को सीमित रखा जाएगा ताकि wildlife और village roads सुरक्षित रहें।

### Nivor Lake Route

यह capital से Miren Lake तक जाने वाली scenic secondary highway होगी। Route farms, orchard villages, dam crossing और lake ferry terminal से गुजरेगी। Fog, rockfall और seasonal water level इस route की travel time बदल सकते हैं।

## Railway network

### Indoru Crown Rail

Crown Rail मुख्य passenger और cargo railway होगी। इसका primary corridor Navaar, Miren Lake, Koral Port और Vaskora Bay को जोड़ेगा। Double-track sections पर passenger trains और freight trains अलग schedules में चलेंगे। Stations में ticketing, platform crowds, cargo loading, police patrols और maintenance crews होंगे।

### Khoruun Freight Line

यह single-track industrial line Navaar को Khoruun City और dry plateau logistics से जोड़ेगी। इसका प्रमुख काम ore, machinery, construction material और water equipment transport होगा। Freight yards में cranes, warehouses, inspection gates और shift-based workers की AI routines होंगी।

### Forest-Lake Rail Loop

Dharvek और Miren Lake के बीच regional rail loop farms, forest edge और normal cities को connect करेगा। इसमें small stations, mixed passenger-cargo trains और slow scenic routes होंगे। Heavy rain या fog के समय signals और departure schedules delay हो सकते हैं।

### Shivren Mountain Rail

यह Crown Highway के साथ चलने वाली mountain rail line होगी। एक long tunnel, snow sheds, rescue sidings और highland station इसके key features होंगे। Winter में line maintenance, snow clearing और emergency evacuation gameplay events हो सकते हैं।

## Air network

छह major cities में बड़े airports होंगे। Navaar International Gateway सबसे बड़ा national hub होगा। Solmera, Mirqara Prime, Vaskora Bay और Velmora Central international-capable airports रखेंगे; Khoruun Ridge Airport cargo और regional aviation पर केंद्रित होगा।

Airport systems में runway operations, terminals, security, baggage, cargo, taxi queues, maintenance, weather delays और NPC travel शामिल होंगे। Air network fast travel देगा, लेकिन storm, fog और airport restrictions के कारण हर route हमेशा available नहीं होगा।

## Ports and ferry routes

Koral Port और Vaskora Bay eastern coastal cargo network के मुख्य ports होंगे। Mirqara Prime inland-sea trade hub होगा। Velmora Central island ferry rings, shipyards और marine rescue network संभालेगा। Mira Coast और small islands तक passenger ferries, cargo boats और local fishing routes चलेंगे।

Ferry schedules tides, wind और storms से प्रभावित होंगे। Port authorities customs, safety inspection, loading queues और emergency closures manage करेंगी।

## NPC traffic and cargo simulation

Traffic को vehicle class और purpose के आधार पर simulate किया जाएगा। Private cars city roads पर चलेंगी; buses fixed routes follow करेंगी; freight trucks industrial corridors पर; police, ambulance और rescue vehicles priority rules use करेंगी; trains timetable और signal blocks follow करेंगी। Active player area में high-detail traffic होगा और दूर के routes aggregate simulation में चलते रहेंगे।

Cargo flows city specialization पर आधारित होंगे। Navaar administrative goods और services भेजेगा। Mirqara और Vaskora समुद्री cargo संभालेंगे। Khoruun mining और machinery भेजेगा। Solmera energy और tourism supplies भेजेगा। Velmora marine goods और island supplies संभालेगा। Villages food, fish, forest products और local craft supply करेंगी।

## Weather and dynamic disruption

Transportation weather-aware होगा। Kora coast में floods और heavy rain, Shivren में snow and ice, Sura में dust storms, Velan में fog and fallen trees, Mira coast में high waves और Nivor में lake fog routes को प्रभावित करेंगे। Closure होने पर road signs, radio, police barricades, train announcements, airport boards और NPC behavior update होंगे।

Game unfair न लगे, इसलिए हर major closure के साथ कम-से-कम एक alternate route, delayed route या public transport option उपलब्ध रहेगा। Severe events special missions और temporary infrastructure repair opportunities बना सकते हैं।

## Emergency and public services

Emergency corridors police command, hospitals, fire/rescue depots, airport emergency services, coast guard, mountain rescue और flood-control offices से जुड़ेंगे। Sirens और priority routing traffic AI को temporarily yield behavior देंगे। Crashes, storms, missing persons, medical calls और infrastructure failures transportation system के dynamic events होंगे।

## Streaming and performance

World को corridor-based streaming cells में विभाजित किया जाएगा। Player के active city और current route के आसपास roads, vehicles, signs, buildings और NPCs high detail में load होंगे। दूर के routes low-detail state में रहेंगे। Railway, cargo और distant traffic को schedule aggregates से simulate किया जा सकता है। इससे map बड़ा दिखेगा, लेकिन memory और CPU load सीमित रहेगा।

## Data source

Machine-readable route definitions `data/transport-network.json` में हैं। हर node और corridor का stable ID, mode, tier, endpoints, capacity/rules और weather risk defined है। Future routes इसी schema से जुड़ेंगे; display names बदलने पर stable IDs और save-game references नहीं बदलने चाहिए।
