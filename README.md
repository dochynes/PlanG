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

- `vendor/` – Původní zdrojové kódy Nauty a Plantri (beze změn). **Pozor: Velikost cca 140 MB.**

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
    return App::run(argc, argv);
}
```

Celá knihovna je navržena okolo tří hlavních callbacků, které uživatel definuje:

1. **`setPreprune` (Heuristika)**  
   Volá se během konstrukce grafu. Umožňuje oříznout větev výpočtu dříve, než je graf hotový.

   - **Vstup:** Částečně vytvořený graf  
   - **Návratová hodnota:**  
     - `1` – PRUNE (zahodit větev)  
     - `0` – KEEP (pokračovat)  
   - **Použití:** Kontrola stupňů, specifické topologické vlastnosti

2. **`setPrune` (Filtr)**  
   Volá se pro hotový graf. Slouží k finální validaci.

   - **Vstup:** Kompletní graf splňující základní parametry generátoru  
   - **Návratová hodnota:**  
     - `1` – PRUNE (zahodit)  
     - `0` – KEEP (ponechat)  
   - **Použití:** Volání složitějších algoritmů (např. Boost coloring, isomorfismus)

3. **`setOutproc` (Výstup)**  
   Volá se pro grafy, které prošly filtrem.

   - **Vstup:** `Output&` (wrapper nad `FILE*`) a graf  
   - **Použití:** Výpis grafu do souboru nebo na `stdout`



**Strategie výstupu:**

Knihovna inteligentně volí způsob výpisu grafů:

1. **Výchozí chování (Bez `setOutproc`):**
   Pokud uživatel callback nenastaví, použije se **nativní formát generátoru** (`graph6` pro Geng, `planar_code` pro Plantri). Výstup směřuje na `stdout` (nebo do souboru specifikovaného v argumentech).

2. **Vlastní výstup (S `setOutproc`):**
   Pokud je callback nastaven, nativní výpis se potlačí a zavolá se vaše funkce.
   - Callback dostává `Output&`, což je bezpečný wrapper nad C `FILE*`.
   - Pokud v argumentech nebyl zadán výstupní soubor, wrapper automaticky píše na `stdout`.

```cpp
App::setOutproc([](auto& out, const auto& g) {
    // Použije operátor<< pro formátovaný výpis (planar_code/graph6)
    out << g;
});

```
### Argumenty příkazové řádky

Použití výsledné aplikace je **identické s originálními nástroji**. Argumenty (`argc`, `argv`) se předávají přímo backendu.

- **Geng:** `./plang -c 5` (vygeneruje souvislé grafy na 5 vrcholech)
- **Plantri:** `./plang -m3 10` (vygeneruje triangulace na 10 vrcholech)

---
V callbacku můžete s grafem pracovat dvěma způsoby:

```cpp
   App::setPrune([](const auto& g) { ... }); // program automaticky dedukuje použitý backend
```
  
  nebo 
```cpp
   using App = Generator<plantri::Backend>; // nebo geng::Backend

App::setPrune([](const App::GraphView& g) {
    ...
});
```

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




Podrobné definice jednotlivých konceptů viz oficiální dokumentace
Boost.Graph:

 https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_concepts.html
---

### Proč `GraphView` není `PropertyGraph`?

Koncept **`PropertyGraph`** v Boostu předpokládá, že vlastnosti
(barva vrcholu, váha hrany, apod.) jsou uloženy **uvnitř grafu**.

`GraphView` je však **read-only pohled** do paměti generátoru,
která je optimalizovaná pro bity a pointery.  
Z tohoto důvodu do ní nelze bezpečně zapisovat uživatelská data.

---

### Řešení: Externí Property Mapy

V souladu s filozofií Boostu používáme **externí property mapy**:

```cpp
// Příklad: Barvení grafu
std::vector<int> colors(num_vertices(g)); // Externí úložiště

auto map = boost::make_iterator_property_map(
    colors.begin(),
    boost::get(boost::vertex_index, g)
);

boost::sequential_vertex_coloring(g, map);
```

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
maticí sousednosti. Navigace je proto zaměřena především na vztahy
sousednosti mezi vrcholy.


TODO: Přidat užitečné funkce



#### Plantri

Plantri je určen pro rovinné grafy a používá reprezentaci typu
**half-edge**, která umožňuje detailní topologickou navigaci v grafu,
včetně práce se stěnami.

Na rozdíl od Gengu Plantri **podporuje i multigrafy (multihrany)**,
pokud je tato možnost povolena přepínači generátoru.

Proto Plantri poskytuje **dodatečné navigační funkce**, které nejsou
k dispozici u Gengu, například:

- `next_edge(e)`
- `prev_edge(e)`
- `opposite_edge(e)`

Tyto operace umožňují procházet hrany okolo vrcholu nebo stěny a jsou
nezbytné pro algoritmy pracující s rovinnou topologií.


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
**TODO:**  rozšířit knihovnu o sadu dalších užitečných pomocných funkcí
