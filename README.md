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

Instalační skript se pokusí Boost automaticky doinstalovat. Na Linuxu k tomu používá `apt-get`, na macOS Homebrew.

Pokud automatická instalace není možná nebo je Boost v nestandardním umístění, spusťte instalaci s proměnnou `BOOST_DIR`:

```bash
BOOST_DIR=/cesta/k/adresari/obsahujicimu/boost ./install.sh


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

Grafy lze filtrovat pomocí callbacku `setPrune`. Callback vrací:

- `KEEP`, pokud má být graf ponechán,
- `PRUNE`, pokud má být graf zahozen.

Jednoduchý příklad:

```cpp
app.setPrune([](const App::GraphView& g) {
    if (condition(g))
        return KEEP;

    return PRUNE;
});
```

Pro předčasné zahazování větví během generování slouží `setPreprune`. To je užitečné hlavně u dražších výpočtů, protože může snížit počet grafů, které generátor musí dokončit.

## Vlastní výpis grafů

Pokud nestačí výchozí výstup, lze nastavit vlastní zpracování přes `setOutproc`:

```cpp
app.setOutproc([](Output& out, const App::GraphView& g) {
    out << "Nalezen graf: ";
    out << g;
});
```

V tomto režimu si uživatel řídí výstup sám.

## Obarvené a zakotvené vrcholy v `geng`

Backend `geng` podporuje také generování grafů s barevnými třídami vrcholů.

Přesné velikosti barevných tříd:

```cpp
app.setVertices(6);
app.setColorClassSizes({4, 2});
```

Intervalové omezení velikostí tříd:

```cpp
app.setVertices(6);
app.setColorClassBounds({
    {2, 4},
    {1, 3}
});
```

Zakotvené vrcholy:

```cpp
app.setVertices(8);
app.setRootedVertices(2);
```

Ukázky použití jsou ve složce `examples/`.


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
