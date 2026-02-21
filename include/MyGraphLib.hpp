#pragma once

#include "common/Generator.hpp"
#include "common/Output.hpp"

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/sequential_vertex_coloring.hpp>
#include <boost/property_map/property_map.hpp> 
#include <boost/range/iterator_range.hpp>

#include "geng/GengAPI.hpp"
#include "geng/GraphViewBoost.hpp" 
#include "geng/GraphViewFunctions.hpp"

#include "plantri/PlantriAPI.hpp"
#include "plantri/GraphViewBoost.hpp" 
#include "plantri/GraphViewFunctions.hpp"

constexpr int KEEP = 0;
constexpr int PRUNE = 1;