#ifdef USE_BOOST_LIBRARIES

#ifndef TOPOLOGYUTILS_H_
#define TOPOLOGYUTILS_H_

#include "ContactPlan.h"
#include <boost/graph/adjacency_list.hpp>
#include "utils/TopologyGraphInfo.h"
#include <fstream>
#include <sstream>
#include <string>
#include <map>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, TopologyVertexInfo, TopologyEdgeInfo, TopologyGraphInfo> TopologyGraph;

namespace topologyUtils
{

/// @brief Compute Topology from the Contact Plan.
/// @return map that associate one TopologyGraph per state
map<double, TopologyGraph> computeTopology(ContactPlan *contactPlan, int nodesNumber);

/// @brief Save Topology in dot and pdf files located in outFileLocation
void saveGraphs(map<double, TopologyGraph> *topology, std::string outFileLocation);

/// @brief Print a Topology State on screen
void printGraph(TopologyGraph topologyGraph);

} /* namespace topologyUtils */

#endif /* TOPOLOGYUTILS_H_ */

#endif /* USE_BOOST_LIBRARIES */
