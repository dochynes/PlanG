#pragma once
#include "PlantriAPI.hpp"
#include "plantri/GraphView.hpp"
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace boost
{

    // Rika Boost.Graph algoritmum, jaky property map typ maji pouzit pro
    // boost::vertex_index_t nad plantri::GraphView. Vlastni mapa je potreba,
    // protoze plantri muze mit v internim poli missing_vertex.
    template<>
    struct property_map<plantri::GraphView,vertex_index_t>
    {
        typedef plantri::vertex_index_map type;
        typedef plantri::vertex_index_map const_type;
    };




};
