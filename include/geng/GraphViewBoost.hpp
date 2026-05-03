#pragma once

#include "geng/GraphView.hpp"

#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace boost
{

template<>
struct property_map<geng::GraphView, vertex_index_t>
{
    typedef identity_property_map type;
    typedef identity_property_map const_type;
};

}
