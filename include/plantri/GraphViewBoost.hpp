#pragma once
#include "PlantriAPI.hpp"
#include "plantri/GraphView.hpp"
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace boost
{

    template<>
    struct property_map<plantri::GraphView,vertex_index_t>
    {
        typedef plantri::vertex_index_map type;
        typedef plantri::vertex_index_map const_type;
    };




};
