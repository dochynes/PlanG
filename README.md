# C++ Graph Generator API (Wrapper pro Nauty/Geng a Plantri)

Tato knihovna poskytuje moderní, sjednocené a typově bezpečné C++20 rozhraní pro dva z nejrychlejších generátorů grafů na světě: Nauty (geng) a Plantri.

Knihovna slouží jako "bridge" (most), který umožňuje vývojářům využívat nízkoúrovňový výkon jazyka C, zatímco píší vysokoúrovňový C++ kód kompatibilní s knihovnou Boost.Graph.

## Klíčové vlastnosti

- **Zero-Copy Overhead**: GraphView je pouze lehká obálka (pohled) nad interní pamětí generátoru. Nedochází k žádnému kopírování dat grafu při předávání do callbacků.
- **Sjednocené API**: Stejná syntaxe pro generování obecných grafů (Geng) i rovinných triangulací (Plantri).
- **Boost.Graph Kompatibilita**: Vygenerované grafy splňují koncepty AdjacencyGraph, VertexListGraph a další, což umožňuje přímé použití algoritmů z `boost::graph` (např. barvení, hledání komponent).
- **Bezpečnost**: Zapouzdření surových C pointerů (zejména u Plantri) do bezpečných iterátorů a navigačních funkcí.

## 1. Architektura

Knihovna není přepsáním generátorů do C++. Místo toho využívá techniku statického linkování s vloženým "shim" vrstvou.

### Jak to funguje pod kapotou

1. **Backend (C)**: Zdrojové kódy geng a plantri jsou zkompilovány jako statické knihovny.
2. **Shim (C/C++ Bridge)**: Malá vrstva (shim), která zpřístupňuje interní, často skryté struktury generátorů (např. bitová pole v Nauty nebo half-edge pointery v Plantri).
3. **Frontend (C++ API)**: Šablonová třída `Generator<Backend>`, která registruje C++ lambda funkce jako callbacky pro C generátor.
4. **GraphView**: Abstraktní pohled na graf. Uživatel v callbacku dostává `const GraphView&`. Tato třída neobsahuje data grafu, ale pouze pointery na data v paměti C generátoru.

### Proč Zero-Copy?

Generátory jako geng mohou produkovat miliony grafů za sekundu. Jakákoliv konverze do objektů typu `std::vector<std::vector<int>>` nebo `boost::adjacency_list` by byla fatální brzdou. Naše řešení umožňuje číst data přímo z interní reprezentace (bitsety pro Geng, pointery pro Plantri) s nulovou režií.

## 2. Kompilace a Struktura projektu

### Závislosti

- Překladač s podporou C++20 (GCC 10+, Clang 11+).
- Boost Graph Libraries.
- Standardní nástroje: `make`, `ar`.

### Struktura adresářů
- `include/` – Veškeré hlavičkové soubory C++ API  
  (`MyGraphLib.hpp` je hlavní vstupní bod)

- `src/` – Implementace wrapperů a C shimů

- `src/geng/` – Backend pro Nauty/Geng

- `src/plantri/` – Backend pro Plantri

- `vendor/` – Původní zdrojové kódy Nauty a Plantri (beze změn). **Pozor: Velikost cca 110 MB.**

- `examples/` – Ukázkové programy (viz níže)

### Sestavení

Projekt používá **Makefile**.  
Pro sestavení knihoven a aplikace spusťte: **make**

Tím se vytvoří statické knihovny:

- `libgeng_wrapper.a`
- `libplantri_wrapper.a`

a také vzorová aplikace.

Pro kompilaci je vyžadována knihovna **Boost** (primárně hlavičkové soubory).
Na systémech Debian/Ubuntu ji nainstalujete příkazem:

```bash
sudo apt-get install libboost-all-dev
```

## 3. Použití a API

Celá knihovna je navržena okolo **tří hlavních callbacků**, které uživatel definuje.

### Inicializace

V souboru `main.cpp` stačí vybrat backend:

```cpp

#include "MyGraphLib.hpp"

// Pro obecné grafy:
using App = Generator<geng::Backend>;

// NEBO pro rovinné grafy:
// using App = Generator<plantri::Backend>;

int main(int argc, char** argv) {
    // ... nastavení callbacků ...
    return App::run();
}
```

Celá knihovna je navržena okolo tří hlavních callbacků, které uživatel definuje:

1. **`setPreprune` (Heuristika)**  
   Volá se během konstrukce grafu. Umožňuje oříznout větev výpočtu dříve, než je graf hotový.

   - **Vstup:** Částečně vytvořený graf  
   - **Návratová hodnota:**  
     -  PRUNE (zahodit větev)  
     -  KEEP (pokračovat)  
   - **Použití:** Kontrola stupňů, specifické topologické vlastnosti

2. **`setPrune` (Filtr)**  
   Volá se pro hotový graf. Slouží k finální validaci.

   - **Vstup:** Kompletní graf splňující základní parametry generátoru  
   - **Návratová hodnota:**  
     -  PRUNE (zahodit)  
     -  KEEP (ponechat)  
   - **Použití:** Volání složitějších algoritmů (např. Boost coloring, isomorfismus)

3. **`setOutproc` (Výstup)**  
   Volá se pro grafy, které prošly filtrem.

   - **Vstup:** `Output&` (wrapper nad `FILE*`) a graf  
   - **Použití:** Výpis grafu do souboru nebo na `stdout`, pokud chce uživatel sám řídit proces výpisu

Priklad použiti: [1](./examples/04_test_geng_planarity_filter.cpp), [2](./examples/02_example_boost.cpp)

## Strategie výstupu

Knihovna flexibilně volí způsob výpisu grafů podle toho, jakou míru kontroly uživatel požaduje.

---
### 1. Nativní výstup (bez `setOutproc`)

Pokud uživatel nenastaví vlastní callback, výpis zajišťuje přímo generátor pomocí svých interních, optimalizovaných C rutin.  
Výstup je směrován na `stdout`, případně do souboru nastaveného pomocí:

```cpp
App::setOutputFile(...);
```
V tomto režimu jsou dostupné všechny nativní formáty generátoru, které lze zvolit pomocí:
```cpp
App::setFormat(App::OutputFormat::...);
```
Dostupné formáty: 
- Plantri: `PlanarCode(default)`,`Graph6`,`Sparse6`,`Ascii`,`EdgeCode`,`DoubleCode`,`NoOutput`
- Geng: `Graph6(default)`, `Sparse6`, `NautyBinary`, `NoOutput `

### 2. Vlastní výstup (pomocí `setOutproc`)

Jakmile je nastaven callback `setOutproc`, nativní výpis generátoru se zcela potlačí a uživatel přebírá plnou odpovědnost za výpis grafů (případně dalších dat nebo statistik).

Callback dostává objekt `Output&`, což je bezpečný C++ wrapper nad nízkoúrovňovým C `FILE*`.

Pokud nebyl přes API(`setOutputFile(string)`) nastaven výstupní soubor, output == `stdout`.

Přiklad:
```cpp
App::setOutproc([](Output& out, const App::GraphView& g) {
    // V tomto bloku máte absolutní kontrolu nad tím, co se zapíše.
    // Můžete vypisovat vlastní texty, statistiky, nebo výsledky algoritmů.
    
    out << "Nalezen graf splňující podmínky: ";
    
    // Použije operátor << pro formátovaný výpis (zatím jen planar_code / graph6)
    out << g;
});
```

> [!NOTE] **Omezení aktuální implementace**

Přetížený operátor výpisu (`out << g`) uvnitř tohoto wrapperu zatím podporuje pouze základní formáty (`planar_code` pro Plantri a `graph6` pro Geng). 
TODO: v budoucich verzich rozšířit implementaci tak, aby `out << g` respektovalo nastavení z `App::setOutput(App::OutputFormat::...)`;



## Konfigurace generátoru Geng

API poskytuje přímé mapování na parametry původního nástroje.  
Pomocí následujících metod generator nastavime přímo z C++.

---

### Velikost generovaného grafu

- `setVertices(int n)` – počet vrcholů
- `setEdgeRange(int min, int max = -1)` – povolený rozsah počtu hran

---

### Strukturální vlastnosti grafu

- `setConnected(bool)`
- `setBiconnected(bool)`
- `setTriangleFree(bool)`
- `setSquareFree(bool)`
- `setBipartite(bool)`
- `setChordal(bool)`
- `setPerfect(bool)`
- `setClawFree(bool)`


---

### Omezení stupňů vrcholů

- `setMinDegree(int)`
- `setMaxDegree(int)`
- `setRegular(bool)`

---

### Nastavení výstupu

- `setFormat(OutputFormat)`
- `setNoOutput()`
- `setOutputFile(std::string)`

---

### Pokročilé volby generátoru

- `setCanonicalLabeling(bool)`
- `setHeader(bool)`
- `setQuiet(bool)`
- `setVerbose(bool)`
- `setSaveMemory(bool)`
- `setDistribution(int res, int mod)`
- `setAdvancedSplit(int)`
- `setAdvancedStartLevel(int)`
- `setSplit(bool)`


## Konfigurace generátoru Plantri

### Velikost grafu

- `setVertices(int)`

### Třída grafu

- `setClass(GraphClass)`
- `setDiskSize(int)`

### Strukturální omezení

- `setMinDegree(int)`
- `setConnectivity(int, bool exact=false)`
- `setMaxFaceSize(int)`

### Výstup

- `setFormat(OutputFormat)`
- `setNoOutput()`
- `setOutputFile(string)`
- `setOutputDual(bool)`

### Symetrie a izomorfismy

- `setOrientationPreserving(bool)`
- `setFullGroup(bool)`
- `setNonTrivialGroup(bool)`

### Paralelizace

- `setDistribution(res, mod)`
- `setSplitLevel(int)`

---
[Priklad použiti](./examples/05_api_usage.cpp)




## 4. Boost.Graph integrace


## Implementované koncepty

`GraphView` splňuje následující koncepty knihovny **Boost.Graph**.
U každého konceptu jsou uvedeny funkce, které jsou dostupné.

### `VertexListGraph`

Umožňuje iteraci přes všechny vrcholy grafu.

Poskytované funkce:

- `vertices(g)`
- `num_vertices(g)`

---

### `EdgeListGraph`

Umožňuje iteraci přes všechny hrany grafu.

Poskytované funkce:

- `edges(g)`
- `num_edges(g)`
- `source(e, g)`
- `target(e, g)`

---

### `AdjacencyGraph`

Umožňuje iteraci přes sousedy daného vrcholu.

Poskytované funkce:

- `adjacent_vertices(v, g)`

---

### `IncidenceGraph`

Umožňuje iteraci přes vycházející hrany daného vrcholu a jeho stupen.

Poskytované funkce:

- `out_edges(v, g)`
- `out_degree(v, g)`
- `source(e, g)`
- `target(e, g)`

### Částečná podpora `PropertyGraph`

`GraphView` neposkytuje plnou implementaci konceptu **PropertyGraph**,  
protože graf je pouze **read-only pohled** do paměti generátoru.

Je však implementována základní vlastnost nutná pro většinu algoritmů Boost: `vertex_index`

```cpp
get(boost::vertex_index, g)
```
Tato funkce vrací property mapu, která každému vrcholu přiřazuje jeho index v rozsahu `0 ... num_vertices(g) - 1`.
Díky tomu lze používat algoritmy, které vyžadují externí úložiště vlastností.

Vlastnosti nejsou uloženy přímo v grafu, protože by to zpomalilo generování grafů kvůli kopírování dat.
Proto optimálni přistup je využit **externí property mapy** z Boost.Graph:

```cpp
std::vector<int> colors(num_vertices(g)); // Boost dokáže přijmout obyčejný vektor. 
//Nebo bezpečnější: boost::make_property_map ...
.
.
.
boost::sequential_vertex_coloring(g,colors.data());

```


Podrobné definice jednotlivých konceptů viz oficiální dokumentace
Boost.Graph:

 https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_concepts.html
---


## 5. Porovnání backendů: Geng vs. Plantri

Ačkoliv API sjednocuje práci s grafy, jednotlivé backendy se pod kapotou
liší **typem podporovaných grafů**, vnitřní reprezentací i dostupnými
navigačními operacemi.

### Společné rozhraní

Oba backendy - **Geng (Nauty)** i **Plantri** - poskytují jednotnou množinu
základních funkcí odpovídajících konceptům knihovny **Boost.Graph**.
Díky tomu lze nad oběma typy grafů psát algoritmy stejným způsobem.

Mezi společně podporované funkce patří zejména:

- `vertices`, `num_vertices`
- `edges`, `num_edges`
- `source`, `target`
- `adjacent_vertices`
- `out_edges`
- `out_degree`

Tyto operace pokrývají obecnou navigaci v grafu a umožňují přímé použití
algoritmů z knihovny Boost.Graph bez ohledu na zvolený backend.


---

### Rozdíly mezi backendy

Přestože základní rozhraní je společné, **ne všechny operace dávají smysl
pro oba typy grafů**.

#### Geng (Nauty)

Backend Geng pracuje s obecnými neorientovanými grafy reprezentovanými
maticí sousednosti.


TODO: Specificke funkce



#### Plantri

Plantri je určen pro rovinné grafy a používá reprezentaci typu
**half-edge**, která umožňuje detailní topologickou navigaci v grafu,
včetně práce se stěnami.

Na rozdíl od Gengu Plantri **podporuje i multigrafy (multihrany)**

Proto Plantri poskytuje **dodatečné navigační funkce**, které nejsou
k dispozici u Gengu, například:

- `next_edge(e)`
- `prev_edge(e)`
- `opposite_edge(e)`
TODO: dalši



## 6. Case Study: Výkon a replikace pluginů

Jedním z hlavních cílů bylo ověřit, že C++ API **výrazně nezpomaluje generování grafů**.

V adresáři: [examples/03_plantri_plugin.cpp](./examples/03_plantri_plugin.cpp) se nachází moje **reimplementace nativního C pluginu `plantri_maxd`** 
(do mého C++ API).Při implementaci jsem se snažil **použít identickou heuristiku Preprune**, jako
v původním C pluginu, a **zachovat logiku filtru maximálního stupně**.

Podle několika testovacích pokusů **vypadá implementace správně a výsledky odpovídají originálu**.


---

### Příklady
Další ukázky použití knihovny lze nalézt ve složce [examples](./examples/)




---


## > [!IMPORTANT] Poznámky a omezení

1) Pokud používáme **Plantri backend** a definujeme vlastní callback pro výstup (`setOutproc`),  
   na konci generování se vždy na `stderr` vypíše diagnostická hláška: `0 triangulations written to stdout;`
   Uživatel by měl tuto hlášku **ignorovat**.  
   Snažil jsem se ji odstranit, ale z důvodu interního fungování Plantri se mi to nepodařilo.  
   **Důvod:** Kvůli "hezkému" API odchytáváme grafy dříve, než projdou nativním I/O systémem Plantri.

2) **missing_vertex**
Interně Plantri obsahuje statickou proměnnou `missing_vertex`, která se používá pouze při generování **polygon triangulací**:
```c
static int missing_vertex = -1;
/* The vertices are numbered 0..nv-1 if missing_vertex<0, and
   0..missing_vertex-1, missing_vertex..nv otherwise.
   This is only used in the code for polygon triangulations. */
```

Ve všech funkcích (kromě `get_property_map`) počítám s tím, že může existovat "chybějící" vrchol,
a všude je ošetřen podmínkou if (missing_vertex ...).

**Poznámka:** Přemýšlím, zda toto větvení může zpomalovat jiné algoritmy pro běžné typy grafů,  
protože mnoho operací musí kontrolovat přítomnost `missing_vertex`, což představuje dodatečné větvení.



