wrapper zpristupnuje generator geng(nauty) pomoci c++ API

generovani probiha stejne jako pri volani nativniho programu geng

knihovna umoznuje pridat vlastni logiku do generovani grafu 
- SetPreprune - levny filtr behem generovani 
- setPrune - filtr nad hotovym grafem 
- setOutproc - uzivatelske zpracovani vystupniho grafu

pokud uzivatel nenastavi vlastni outproc, pouzije se puvodni vystup (graph6)

vsechny callbacky dostavaji graf jako "const GraphView&" , (uzivatel nemusi znat presny typ, muze pouzit auto)
je to read-only zero-copy view na vnitrni bitovou representaci grafu v nauty.
GraphView implementuje nasledujici Boost.Graph koncepty: AdjacencyGraph,IncidenceGraph,VertexListGraph,EdgeListGraph (https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_concepts.html)

diky tem konceptum nad vygenerovanym grafem muzeme volat funkce a algoritmy z knihovny boost :num_vertices(g), num_edges(g), 
vertices(g), edges(g), out_degree(v, g), adjacent_vertices(v, g),bfs,dfs ... 

graf je vzdy jednoduchy, neorientovany, vrcholy 0...n-1

PropertyGraph(https://www.boost.org/doc/libs/latest/libs/graph/doc/graph_concepts.html) jsem neimplementoval. 
Myslim ze by to zbytecne komplikovalo a slo proti zero-copy view
Misto toho, kdyz uzivatel potrebuje uchovat nejake vlastnosti(barvy,vhay...) by mel pouzit externi property mapy z boostu(jako jsou iterator_property_map, vector_property_map,associative_property_map ...)

example viz ./src/geng_demo.cpp