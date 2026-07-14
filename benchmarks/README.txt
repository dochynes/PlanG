Benchmarky pro mereni vykonu
================================

Adresar obsahuje pouze benchmarkovaci programy napsane nad knihovnou PlanG,
ktere byly pouzity pri mereni vykonu v praci:

- bench_geng
- bench_plantri
- dump_compare
- count_plantri_maxdeg

Programy se sestavi prikazem:

    make

Benchmark bench_geng meri pouziti backendu geng pres knihovnu PlanG.
Benchmark bench_plantri meri pouziti backendu plantri pres knihovnu PlanG.
Program dump_compare uklada vystup PlanG do souboru, aby jej bylo mozne
porovnat s vystupem puvodnich generatoru. Program count_plantri_maxdeg pocita
triangulace s omezenim maximalniho stupne pomoci callbacku v PlanG; odpovida
tak testu zalozenemu na pluginu maxdeg.c pro puvodni plantri.

Pri mereni byly PlanG benchmarky prekladany jako C++20 programy s optimalizacnimi
flagy:

    -O3 -flto -march=native -DNDEBUG

Referencni spusteni puvodnich generatoru byla provedena mimo tento Makefile se
stejnymi optimalizacnimi flagy. Backend geng v PlanG se sestavuje s nastavenim
WORDSIZE=64 a MAXN=WORDSIZE. Hodnota WORDSIZE urcuje velikost bitoveho slova,
ktere nauty pouziva pro reprezentaci mnozin vrcholu a radku matice sousednosti.
Varianta WORDSIZE=64 odpovida beznym 64bitovym pocitacum a dovoluje v teto
jednoslovove reprezentaci pracovat s grafy az do 64 vrcholu.

Pri porovnani s puvodnim geng je proto potreba pouzit stejnou konfiguraci,
napriklad nativni program gengL.

U testu maximalniho stupne byl puvodni plantri sestaven s pluginem maxdeg.c.
