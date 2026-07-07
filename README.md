# PlanG

PlanG je konzolová aplikace a C++ knihovna pro generování grafů. Umožňuje používat generátory `geng` a `plantri` přes jednotné C++ rozhraní a výsledné grafy dále filtrovat nebo zpracovávat vlastním kódem.

Program je určen hlavně pro experimenty v teorii grafů, testování algoritmů a generování množin grafů s danými vlastnostmi.

## Co program umí

PlanG zpřístupňuje dva specializované generátory grafů v jednom prostředí. Uživatel si v C++ kódu zvolí, zda chce pracovat s obecnými grafy generovanými pomocí `geng`, nebo s rovinnými grafy generovanými pomocí `plantri`.

Program umožňuje nastavit základní vlastnosti generovaných grafů, například počet vrcholů, počet hran, stupňová omezení nebo vybrané strukturální podmínky. Vygenerované grafy lze dále filtrovat vlastním C++ kódem a výsledky ukládat ve standardních formátech( například `graph6` nebo `sparse6`).

Součástí projektu jsou také ukázkové programy ve složce `examples/`, které předvádějí typické způsoby použití knihovny.

## Požadavky

### Hardware

Pro sestavení projektu a spouštění běžných experimentálních příkladů stačí běžný 64bitový počítač, alespoň 4 GB RAM, alespoň 500 MB volného místa na disku:


### Software

Projekt lze sestavit na Linuxu a macOS. Na Windows je doporučené použít WSL.
Na Linuxu se ve výchozím nastavení používá optimalizované sestavení přes GCC. Na macOS instalační skript automaticky použije `clang` a `clang++`, aby bylo sestavení kompatibilní se standardními vývojářskými nástroji od Applu.

Je potřeba:

- překladač s podporou C++20, například GCC 10+ nebo Clang 11+,
- `make`,
- `ar`,
- knihovna Boost, především Boost.Graph; instalační skript ji umí doinstalovat automaticky,

Na macOS je před spuštěním instalačního skriptu potřeba mít nainstalované vývojářské nástroje a Homebrew.
Na Debianu, Ubuntu nebo WSL se skript pokusí potřebné build nástroje doinstalovat přes `apt-get`.

## Instalace


```bash
./install.sh
```

Skript zkontroluje základní nástroje, v případě potřeby doinstaluje Boost a spustí sestavení projektu. Na Debianu, Ubuntu nebo WSL používá `apt-get`, na macOS používá Homebrew.

Po úspěšném dokončení vznikne spustitelný soubor:

```bash
./plang
```


### Chybí Boost

Instalační skript se pokusí Boost automaticky doinstalovat.

Pokud Boost je v nestandardním umístění, spusťte instalaci s proměnnou `BOOST_DIR`:

```bash
BOOST_DIR=/cesta/k/adresari/obsahujicimu/boost ./install.sh
```


## Spuštění

Po sestavení spusťte program:

```bash
./plang
```

Konkrétní chování programu je určeno zdrojovým souborem `main.cpp`. V něm se nastavuje, který generátor se použije a jaké grafy se mají generovat.

Například ukázkový program může generovat rovinné grafy na 7 vrcholech:

```cpp
#include "MyGraphLib.hpp"

using App = Generator<plantri::Backend>;

int main()
{
    App app;

    app.setVertices(7);
    return app.run();
}
```

Po změně `main.cpp` je potřeba projekt znovu sestavit:

```bash
./install.sh
```

## Výběr generátoru

Základní třídou knihovny je šablona `Generator`. Do špičatých závorek se zadává backend, který určuje, jaký původní generátor se použije.

Pro obecné grafy:

```cpp
Generator<geng::Backend> app;
```

Pro rovinné grafy:

```cpp
Generator<plantri::Backend> app;
```

Pro kratší zápis si můžeme vytvořit typovou zkratku, například:

```cpp
using App = Generator<geng::Backend>;
App app;
```

V následujících ukázkách se používá proměnná `app`, která představuje konkrétní instanci generátoru.

## Základní nastavení generování

Počet vrcholů:

```cpp
app.setVertices(10);
```

Rozsah počtu hran pro `geng`:

```cpp
app.setEdgeRange(10, 15);
```

Požadavek na souvislé grafy:

```cpp
app.setConnected();
```

Požadavek na bipartitní grafy:

```cpp
app.setBipartite();
```

Zákaz trojúhelníků:

```cpp
app.setTriangleFree();
```

Nastavení výstupu do souboru:

```cpp
app.setOutputFile("output.g6");
```

### Další volby pro `geng`

Backend `geng` se používá pro obecné neorientované grafy. Lze u něj nastavovat například:

| Metoda | Význam |
| --- | --- |
| `setConnected()` | generuje pouze souvislé grafy |
| `setBiconnected()` | generuje pouze 2-souvislé grafy |
| `setBipartite()` | generuje pouze bipartitní grafy |
| `setTriangleFree()` | zakáže trojúhelníky |
| `setSquareFree()` | zakáže čtyřcykly |
| `setMinDegree(d)` | nastaví minimální stupeň vrcholu |
| `setMaxDegree(d)` | nastaví maximální stupeň vrcholu |
| `setRegular()` | generuje regulární grafy |
| `setCanonicalLabeling()` | zapne kanonické přeznačení výstupu |

Příklad:

```cpp
Generator<geng::Backend> app;

app.setVertices(10);
app.setConnected();
app.setTriangleFree();
app.setEdgeRange(12, 18);

return app.run();
```

### Další volby pro `plantri`

Backend `plantri` se používá pro rovinné grafy. Lze u něj nastavovat například:

| Metoda | Význam |
| --- | --- |
| `setDiskSize(n)` | generuje triangulace disku s vnější stěnou velikosti `n` |
| `setMinDegree(d)` | nastaví minimální stupeň vrcholu |
| `setConnectivity(c)` | nastaví požadovanou konektivitu |
| `setMaxFaceSize(f)` | omezí maximální velikost stěny |
| `setOutputDual()` | vypisuje duální grafy |
| `setOrientationPreserving()` | vypíše jednu reprezentaci pro každou orientační třídu |
| `setFullGroup()` | zapne výpočet celé automorfismové grupy |

Příklad:

```cpp
Generator<plantri::Backend> app;

app.setVertices(7);
app.setDiskSize(3);
app.setHeader();

return app.run();
```

### Rozdělení výpočtu

Oba backendy podporují rozdělení výpočtu na části pomocí `setDistribution(res, mod)`. To se hodí, když chceme větší generování rozdělit do více samostatných běhů:

```cpp
app.setDistribution(0, 4);
```

Tento příklad spustí první ze čtyř částí výpočtu. Další běhy by použily hodnoty `1, 4`, `2, 4` a `3, 4`.

## Výstup

Bez vlastního zpracování se vygenerované grafy vypisují na standardní výstup nebo do souboru nastaveného pomocí:

```cpp
app.setOutputFile("output.g6");
```

U `geng` je výchozí formát `graph6`. Lze použít například také `sparse6`:

```cpp
app.setFormat(App::OutputFormat::Sparse6);
```

Výstup lze vypnout:

```cpp
app.setNoOutput();
```

## Vlastní filtrování grafů

Dokončené grafy lze filtrovat pomocí callbacku `setFilter`. Callback vrací:

- `KEEP`, pokud má být graf ponechán,
- `PRUNE`, pokud má být graf zahozen.

Jednoduchý příklad:

```cpp
app.setFilter([](const App::GraphView& g) {
    if (condition(g))
        return KEEP;

    return PRUNE;
});
```

Pro předčasné zahazování větví během generování slouží `setPreprune`. To je užitečné hlavně u dražších výpočtů, protože může snížit počet grafů, které generátor musí dokončit.

Příklad předčasného filtrování:

```cpp
app.setPreprune([](const App::GraphView& g) {
    if (cast_vypoctu_nema_smysl_dokoncovat(g))
        return PRUNE;

    return KEEP;
});
```

## Práce s grafem v callbacku

V callbacku dostáváme objekt `GraphView`, který umožňuje číst právě generovaný graf. Pro běžné algoritmy lze používat funkce známé z Boost.Graph, například `num_vertices(g)`, `vertices(g)`, `edges(g)`, `out_edges(v, g)` nebo `out_degree(v, g)`.

U backendu `plantri` jsou navíc dostupné pomocné funkce pro práci s orientovanými hranami v planární reprezentaci:

| Funkce | Význam |
| --- | --- |
| `next_edge(e)` | přejde na další orientovanou hranu |
| `prev_edge(e)` | přejde na předchozí orientovanou hranu |
| `opposite_edge(e)` | přejde na stejnou hranu v opačném směru |
| `source(e, g)` | vrátí počáteční vrchol hrany |
| `target(e, g)` | vrátí koncový vrchol hrany |

Příklad použití u `plantri`:

```cpp
auto [begin, end] = out_edges(v, g);

for (auto it = begin; it != end; ++it)
{
    auto e = *it;
    auto opposite = opposite_edge(e);
    auto next = next_edge(opposite);

    int u = target(next, g);
}
```

## Vlastní výpis grafů

Pokud nestačí výchozí výstup, lze nastavit vlastní zpracování přes `setOutproc`:

```cpp
app.setOutproc([](Output& out, const App::GraphView& g) {
    out << "Nalezen graf: ";
    out << g;
});
```

V tomto režimu si uživatel řídí výstup sám.

Callback `setOutproc` se používá také tehdy, když nechceme grafy jen vypsat, ale například počítat statistiky nebo spouštět vlastní algoritmus nad každým nalezeným grafem.

```cpp
int count = 0;

app.setOutproc([&](Output&, const App::GraphView&) {
    ++count;
});

app.run();
```

## Obarvené a zakotvené vrcholy v `geng`

Backend `geng` podporuje také generování grafů s barevnými třídami vrcholů. Izomorfismus pak musí respektovat barvy: vrcholy stejné barvy se mohou zaměňovat, ale vrcholy různých barev ne.

Barvy jsou rozlišené. To znamená, že například rozklad `{1, 3}` není totéž jako `{3, 1}`.

Přesné velikosti barevných tříd:

```cpp
app.setVertices(6);
app.setColorClassSizes({4, 2});
```

Tento příklad generuje grafy na 6 vrcholech, kde barva 0 má 4 vrcholy a barva 1 má 2 vrcholy.

Intervalové omezení velikostí tříd:

```cpp
app.setVertices(6);
app.setColorClassBounds({
    {2, 4},
    {1, 3}
});
```

Tento příklad připouští všechny rozklady počtu vrcholů, kde barva 0 má 2 až 4 vrcholy a barva 1 má 1 až 3 vrcholy.

Pokud chceme zadat jen počet barev a ne jejich přesné velikosti, použijeme:

```cpp
app.setVertices(4);
app.setColors(2);
```

Pro 4 vrcholy se tím připustí například rozklady `{1, 3}`, `{2, 2}` a `{3, 1}`.

Zakotvené vrcholy:

```cpp
app.setVertices(8);
app.setRootedVertices(2);
```

Zakotvené vrcholy jsou zvláštní případ barev. Předchozí příklad interně odpovídá rozkladu `{6, 1, 1}`: šest běžných vrcholů a dva vrcholy se samostatnou barvou. Díky tomu je izomorfismus nesmí zaměnit s běžnými vrcholy ani mezi sebou.

### Přístup k barvám v callbacku

V callbackách `setFilter`, `setPreprune` a `setOutproc` lze barvy číst přes `GraphView`:

```cpp
app.setFilter([](const App::GraphView& g) {
    if (!g.has_coloring())
        return KEEP;

    int c = g.color(0);
    auto colors = g.vertex_colors();

    return KEEP;
});
```

| Metoda | Význam |
| --- | --- |
| `g.has_coloring()` | vrátí, zda je obarvení aktivní |
| `g.color_count()` | počet barev |
| `g.color(v)` | barva vrcholu `v`, nebo `-1`, pokud není dostupná |
| `g.vertex_colors()` | seznam barev všech vrcholů |
| `g.vertices_of_color(c)` | seznam vrcholů barvy `c` |

### Výstup obarvených grafů

Standardní formáty `graph6` a `sparse6` samy o sobě neobsahují informace o barvách. Pokud je obarvení aktivní a není nastavený vlastní `setOutproc`, výchozí výstup za každý graf dopíše i barvy vrcholů:

```text
C? colors: 0 1 1
```



Pokud chceme, aby se výstupní grafy před vypsáním kanonicky přeznačily, zavoláme:

```cpp
app.setCanonicalLabeling();
```

Při kanonickém přeznačení se přeuspořádají i barvy vrcholů.


## Více konfigurací v jednom programu

Každý objekt `Generator` má vlastní nastavení. V jednom programu proto můžeme připravit více generátorů a spustit je postupně:

```cpp
Generator<geng::Backend> connected;
connected.setVertices(8);
connected.setConnected();

Generator<geng::Backend> triangle_free;
triangle_free.setVertices(8);
triangle_free.setTriangleFree();

connected.run();
triangle_free.run();
```

Paralelní běhy stejného backendu nejsou podporované, protože původní generátory, používají uvnitř globální stav.



## Čištění projektu

Vygenerované knihovny a spustitelný soubor lze odstranit příkazem:

```bash
make clean
```


## Struktura projektu

- `main.cpp` - hlavní ukázkový program, který se sestaví do `plang`,
- `examples/` - ukázky použití,
- `include/` - veřejné hlavičkové soubory knihovny,
- `src/` - zdrojové soubory wrapperu,
- `vendor/` - přiložené zdrojové kódy generátorů `nauty/geng` a `plantri`,
- `Makefile` - sestavení projektu,
- `install.sh` - jednoduché sestavení jedním příkazem.
