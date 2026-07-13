#pragma once

#include "geng/GraphView.hpp"

#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace boost
{

// Rika Boost.Graph algoritmum, jakou property mapu maji pouzit pro
// boost::vertex_index_t nad geng::GraphView.
// Vrcholy geng grafu jsou cislovane souvisle 0..n-1, proto staci identity mapa.
template<>
struct property_map<geng::GraphView, vertex_index_t>
{
    typedef identity_property_map type;
    typedef identity_property_map const_type;
};

}
