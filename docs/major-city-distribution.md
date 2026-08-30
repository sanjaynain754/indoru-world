# Indoru Major City Distribution and Infrastructure

## Design principle

Indoru की छह major cities को map के चारों ओर दूर-दूर रखा गया है। वे एक ही urban cluster में नहीं होंगी। हर city अलग terrain और climate zone से जुड़ी होगी, इसलिए player को अलग visual identity, population behavior, airport experience, government services और road network मिलेगा। सभी cities **Indoru country** के अंदर हैं और एक ही Indoru national flag use करती हैं।

## Geographic distribution

| City | Map position | Geographic zone | Strategic purpose |
|---|---|---|---|
| **Navaar** | central basin | river, warm-temperate plains | national capital and all-country hub |
| **Solmera** | southeast perimeter | warm coast, lagoons and low hills | tourism, energy and sea gateway |
| **Mirqara Prime** | western inland-sea inlet | maritime terraces and deep port | trade, customs and shipping |
| **Khoruun City** | northwest dry plateau | canyon mouth and high dryland | freight, mining and engineering |
| **Vaskora Bay** | eastern rain coast | storm bay, canals and breakwaters | shipbuilding and disaster response |
| **Velmora Central** | northeast island arc | connected islands and deep channels | ferries, marine trade and island governance |

Navaar से बाकी पाँच cities तक direct route होगा, लेकिन travel short नहीं रखा जाएगा। Ring road और central highways केवल main corridors होंगे; terrain, weather और road choices journey को meaningful बनाएँगे। कोई major city दूसरे major city के district के अंदर नहीं होगी।

## Major-city standards

छहों major cities में normal cities की तुलना में अधिक population density, wider roads, multi-level junctions, reliable public services, advanced hospitals, dense NPC schedules, high-rise और landmark buildings, large police presence, government offices और fully developed transport systems होंगे। हर city में एक बड़ा airport होगा। Airport केवल decoration नहीं होगा; flights, cargo, taxis, security, luggage workers, maintenance crews और weather delays simulation का हिस्सा होंगे।

## Road and transport network

Indoru की primary network में six-city radial highways, Capital Ring Road, East Rain Expressway, Southern Desert Bypass, Western Forest Route, Northern Ridge Highway और Coastal Ferry Network शामिल होंगे। Roads को तीन levels में रखा जाएगा: primary expressways major cities को जोड़ेंगी; secondary roads normal cities और villages तक जाएँगी; local roads farms, neighborhoods, quarries, ports और mountain settlements को connect करेंगी।

Airport access के लिए अलग express routes होंगे। Government convoys, emergency vehicles, freight trucks, buses और ordinary NPC traffic के लिए route priorities अलग रहेंगी। Storms, snow, dust storms, accidents और construction के कारण navigation system alternate paths सुझाएगा।

## Population and service hierarchy

Navaar और Vaskora Bay में very-high population simulation होगी। Solmera, Mirqara Prime और Velmora Central high-density metropolitan cities होंगी। Khoruun City high-density industrial city होगी। Major city population को districts में बाँटा जाएगा—commercial core, residential zones, government quarter, industrial area, transport hub और outer settlements। Population का बड़ा भाग static crowd नहीं होगा; residents work, school, shopping, travel, healthcare और public events के schedules follow करेंगे।

हर major city में कम-से-कम एक central police command, multiple local stations, major hospital, emergency response center, fire/rescue service, court or civic legal office, government administration, public transport hub, university or training center, stadium/cultural venue, cargo facility और airport authority होगी। अलग cities में services का emphasis अलग रहेगा।

## City profiles

### Navaar

Navaar central river basin में national capital है। इसका airport **Navaar International Gateway** चार runways, international routes और national cargo terminal के साथ सबसे बड़ा होगा। Capital में parliament, supreme court, civil registry, central police command, national hospital, central railway hub, metro corridors, ring road, river bridges और airport expressway होंगे। यहाँ NPC density सबसे अधिक और public services सबसे complete होंगी।

### Solmera

Solmera southeast warm coast पर है। **Solmera Sunport** tourism, cargo और international flights संभालेगा। Coastal expressway, solar transit corridor, harbor road, ferry terminals और storm shelters city को support करेंगे। Solmera की major buildings hotels, clean-energy towers, ocean research offices, hospitals और public beach infrastructure होंगी।

### Mirqara Prime

Mirqara Prime western inland-sea inlet पर trade metropolis है। **Mirqara International Air-Sea Hub** airport को deep-water harbor, cargo railway, container yards और customs corridor से जोड़ेगा। City में customs authority, trade court, port security command, merchant licensing office और marine hospital होंगे। यहाँ freight, customs, shipping और marketplace NPCs की density अधिक होगी।

### Khoruun City

Khoruun City northwest red canyon mouth पर industrial major city है। **Khoruun Ridge Airport** highland edge पर होगा और cargo तथा mining logistics संभालेगा। Three-level canyon roads, freight railway, mountain tunnel, desert bypass, quarry roads और elevated bridges इसके infrastructure anchors होंगे। City में transport authority, mining inspectorate, rescue command, trauma hospital और heavy-vehicle services प्रमुख होंगी।

### Vaskora Bay

Vaskora Bay eastern rain coast की deep crescent bay पर है। **Vaskora Bay Aerodrome** storm-aware flight operations, cargo और emergency logistics के लिए बनाया जाएगा। Storm barriers, raised expressways, canal bridges, flood-safe rail, shipyards और airport causeway city की मुख्य infrastructure होगी। Storm authority, flood-control office, coast guard, emergency hospitals और weather observatory यहाँ की signature services होंगी।

### Velmora Central

Velmora Central northeast island arc में कई connected islands पर फैली metropolis है। **Velmora Sky-Island Airport** bridges, ferry rings, elevated rail और harbor tunnels से जुड़ा होगा। City का government model island council और maritime authority पर आधारित होगा। Ferry police, rescue fleet command, port customs, marine hospital, shipyards और island logistics इसके प्रमुख service systems होंगे।

## Runtime and load policy

Major cities को separate streaming cells में बाँटा जाएगा। Player के active city district में high-detail buildings, pedestrians, traffic और service AI load होंगे। दूर के districts simplified simulation में रहेंगे। Airports, government buildings और major landmarks priority assets होंगे। NPC density को hardware tier के अनुसार scalable रखा जाएगा, पर city identity और critical gameplay roles हमेशा preserved रहेंगे।

## Content priority

पहले playable build में Navaar के selected districts और बाकी major cities के low-detail approach zones बनाए जाएँगे। बाद में प्रत्येक major city का complete airport, service district, residential network और landmark set unlock होगा। इस तरह map का world-scale impression शुरू से रहेगा, लेकिन development team quality और performance के साथ city-by-city विस्तार कर सकेगी।
