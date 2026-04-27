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

2. **`setPrune` (Pruning / filtr)**  
   V backendu `geng` se volá i pro mezistavy generování, tedy také pro grafy s `g.num_vertices() < g.maxn()`.

   - **Vstup:** Částečně vytvořený nebo finální graf  
   - **Návratová hodnota:**  
     -  PRUNE (zahodit)  
     -  KEEP (ponechat)  
   - **Použití:** Volání složitějších algoritmů (např. Boost coloring, isomorfismus)


   Pokud chce mít uživatel jistotu, že pracuje pouze s finální verzí grafu, měl by graf zpracovat v `setOutproc`.

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

- `distance_between`




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


1) **missing_vertex**
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





## Obarvené grafy v `geng`

Aktuální implementace rozšiřuje `geng` o generování grafů vzhledem k barevným třídám vrcholů. Základní myšlenka je, že izomorfismus už nesmí libovolně permutovat všechny vrcholy, ale musí respektovat barvy. Vrcholy stejné barvy se mezi sebou mohou zaměňovat, ale vrcholy různých barev ne.

Interně se obarvení sleduje pomocí barevných tříd. Každý vrchol má přiřazený index barvy a generátor průběžně hlídá, kolik vrcholů už v jednotlivých barvách vzniklo.

```text
barva 0: aktuální počet vrcholů v barvě 0
barva 1: aktuální počet vrcholů v barvě 1
...
barva k-1: aktuální počet vrcholů v barvě k-1
```

Omezení na velikosti tříd lze zadat přesně, například `{4,2}`, nebo intervalově, například `barva 0: 2..4`, `barva 1: 1..3`. Pro každý finální graf musí součet skutečných velikostí tříd odpovídat počtu vrcholů grafu.

### Jak probíhá barvení v `geng`

Původní `geng` generuje grafy postupným přidáváním vrcholů. Úprava přidává k tomuto procesu ještě průběžné přiřazování barvy právě přidanému vrcholu.

Zjednodušený pseudokód:

```text
extend(graf G na n vrcholech):
    pro každou možnou množinu sousedů x nového vrcholu:
        pokud colouring není aktivní:
            pokračuj původním geng algoritmem

        pokud colouring je aktivní:
            pro každou barvu c:
                přiřaď novému vrcholu barvu c
                zvyš aktuální počet vrcholů barvy c

                pokud barevné počty ještě mohou splnit zadané meze tříd:
                    proveď accept test s partition rozdělenou podle barev
                    pokud test projde:
                        pokračuj rekurzivně

                vrať přiřazení barvy zpět
```


Kontrola realizovatelnosti barev se dělá pomocí dolních a horních mezí tříd. Pokud například generujeme přesné třídy `{3,1}`, interně se to chápe jako intervaly `3..3` a `1..1`. Žádná větev generování nesmí překročit horní mez a zároveň se průběžně kontroluje, že ze zbývajících vrcholů ještě lze splnit všechny dolní meze.

Před voláním `nauty` se počáteční partition rozdělí podle barev. Tím `nauty` dostane informaci, které vrcholy patří do stejné barevné třídy, a canonical generation potom pracuje s barevně zachovávajícími izomorfismy.

### Přístup k barvám v callbackách

Uživatel má v `prune` a `preprune` přístup k aktuálním barvám přes `GraphView`:

```cpp
App::setPreprune([](const App::GraphView& g) {

    if (!g.has_coloring())
        return 0;

    int color = g.color(0);
    return 0;
});
```

Dostupné metody:

```cpp
g.has_coloring()
g.color_count()
g.color(v)
g.vertex_colors()
g.vertices_of_color(color)
```

`g.color(v)` vrací index barvy vrcholu `v`. Pokud obarvení není aktivní nebo je vrchol mimo rozsah, vrací `-1`.
`g.vertex_colors()` vrací pole velikosti `g.num_vertices()`, kde na indexu `v` je barva vrcholu `v`.
`g.vertices_of_color(color)` vrací seznam vrcholů, které mají zadanou barvu.

V `setPrune` a `setPreprune` barvy odpovídají aktuálnímu grafu, se kterým callback pracuje. V `setOutproc` barvy odpovídají grafu, který se právě vypisuje. Pokud je zapnuté `App::setCanonicalLabeling()`, graf se před výstupem kanonicky přeznačí a barvy se přeuspořádají stejným způsobem.

### `App::setColorClassSizes(...)`

Nejzákladnější způsob použití je přímo zadat velikosti jednotlivých barevných tříd.

```cpp
using App = Generator<geng::Backend>;

int main()
{
    App::setVertices(6);
    App::setColorClassSizes({4, 2});
    return App::run();
}
```

Tento příklad generuje grafy na 6 vrcholech se dvěma barevnými třídami:

```text
barva 0: 4 vrcholy
barva 1: 2 vrcholy
```

Barvy jsou v tomto režimu rozlišené. To znamená, že barva `0` a barva `1` mají konkrétní význam a izomorfismus je nesmí mezi sebou prohodit.

Ukázka použití s filtrováním podle hran mezi barevnými třídami je v [examples/07_color_class_sizes_prune.cpp](./examples/07_color_class_sizes_prune.cpp).

`{1,3}` a `{3,1}` jsou pro rozlišené barvy dva různé případy:

```text
{1,3}: barva 0 má 1 vrchol, barva 1 má 3 vrcholy
{3,1}: barva 0 má 3 vrcholy, barva 1 má 1 vrchol
```

### `App::setColorClassBounds(...)`

Obecnější varianta dovoluje pro každou barevnou třídu zadat dolní a horní mez její velikosti.

```cpp
App::setVertices(6);
App::setColorClassBounds({
    {2, 4},
    {1, 3}
});
```

To znamená:

```text
barva 0: 2 až 4 vrcholy
barva 1: 1 až 3 vrcholy
```

Generátor pak připouští všechny přesné rozklady, které tyto meze splňují a jejichž součet je `n`. Pro `n = 6` tedy například:

```text
{3,3}
{4,2}
```

Funkce `setColorClassSizes(...)` je speciální případ této obecnější varianty, kde pro každou třídu platí dolní mez = horní mez.

### `App::setRootedVertices(k)`

Zakotvené vrcholy jsou implementované jako speciální případ barevných tříd. Pokud chceme zakotvit `k` vrcholů na grafu s `n` vrcholy, interně se vytvoří rozklad:

```text
{n-k, 1, 1, ..., 1}
```

Tedy jedna velká třída obyčejných vrcholů a `k` singleton tříd. Každý zakotvený vrchol má vlastní unikátní barvu.

Příklad:

```cpp
App::setVertices(8);
App::setRootedVertices(2);
```

Interně odpovídá:

```cpp
App::setColorClassSizes({6, 1, 1});
```

Význam:

```text
barva 0: 6 obyčejných vrcholů
barva 1: první zakotvený vrchol
barva 2: druhý zakotvený vrchol
```

Tím se realizuje generování grafů, kde `k` vrcholů má speciální roli a izomorfismus je nesmí zaměnit s běžnými vrcholy.

Ukázka použití se dvěma zakotvenými vrcholy a filtrem na jejich vzdálenost : [examples/08_rooted_vertices_prune.cpp](./examples/08_rooted_vertices_prune.cpp).

### `App::setColors(k)`

Funkce `setColors(k)` slouží pro případ, kdy uživatel chce generovat grafy s přesně `k` rozlišenými barvami, ale nechce ručně zadávat velikosti tříd.

V aktuální implementaci se `setColors(k)` interně převádí na intervalové omezení:

```text
barva 0: 1 až n vrcholů
barva 1: 1 až n vrcholů
...
barva k-1: 1 až n vrcholů
```

Tím se v jednom běhu připustí právě všechny rozklady počtu vrcholů `n` do `k` nenulových uspořádaných tříd.

Příklad:

```cpp
App::setVertices(4);
App::setColors(2);
```

Interně se připustí například tyto konkrétní velikosti tříd:

```text
{1,3}
{2,2}
{3,1}
```


Ukázka použití `setColors(2)` s filtrem nad velikostí jedné barevné třídy je v [examples/06_colored_geng.cpp](./examples/06_colored_geng.cpp).




### Poznámka k výstupu

Standardní `graph6` ani `sparse6` formát sám o sobě neobsahuje informace o barvách. Pokud je ale v této knihovně aktivní obarvení a uživatel nenastaví vlastní `setOutproc`, výchozí výstup za graf dopíše i barvy vrcholů:

```text
C? colors: 0 1 1
```

Pokud uživatel nastaví vlastní `setOutproc`, je formát výstupu plně v jeho režii.

Pokud uživatel chce, aby se výstupní grafy před vypsáním kanonicky přeznačily, může zavolat:

```cpp
App::setCanonicalLabeling();
```
Při kanonickém přeznačení se současně přeuspořádají i barvy vrcholů, takže výchozí barevný výstup i `GraphView` v `setOutproc` odpovídají výstupnímu pořadí vrcholů.




Obarvení je napojené i na omezenou větev `spaextend()`. Barvy jsou tedy dostupné také při použití přepínačů, které `geng` interně vedou přes `spaextend()`, například `-m`, `-t` nebo `-f`.

Ukázka kombinace `setTriangleFree()`, `setRootedVertices(1)`  je v [examples/09_spaextend_coloring.cpp](./examples/09_spaextend_coloring.cpp).
