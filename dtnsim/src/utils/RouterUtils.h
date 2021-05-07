//#include "Config.h"

#ifdef USE_BOOST_LIBRARIES

#ifndef ROUTERUTILS_H_
#define ROUTERUTILS_H_

#include "TopologyUtils.h"
#include "ContactPlan.h"
#include <boost/graph/adjacency_list.hpp>
#include "RouterGraphInfo.h"
#include <string>
#include <fstream>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, RouterVertexInfo, RouterEdgeInfo, RouterGraphInfo> RouterGraph;

namespace routerUtils
{

	/// @brief Compute Flows of traffic from the BundleMaps files.
	/// @return map that associate one RouterGraph per state
	map<double, RouterGraph> computeFlows(ContactPlan *contactPlan, int nodesNumber, string bundleMapsLocation);

	/// @brief Save Flows in dot and pdf files located in outFileLocation
	void saveGraphs(ContactPlan *contactPlan, map<double, RouterGraph> *flows, vector<string> dotColors, map<pair<int, int>, unsigned int> flowIds, std::string outFileLocation);

	/// @brief Print a RoterGraph with the flows per State on screen
	void printGraph(RouterGraph routerGraph);

	/// @brief Gets a vector with string corresponding to colors
	/// usable to write files in dot format
	vector<string> getDotColors();

	/// @brief return used Contacts capacities in bytes
	/// map[contactId, txBytes]
	map<int, double> getUsedContacts(ContactPlan *contactPlan, map<double, RouterGraph> *flows);

	/// @brief Gets delivery time of the last delivered traffic
	double getMaxDeliveryTime(ContactPlan *contactPlan, map<double, RouterGraph> *flows);

	/// @brief Gets total transmissions in bytes
	double getTotalTxBytes(ContactPlan *contactPlan, map<double, RouterGraph> *flows);

} /* namespace routerUtils */

#endif /* ROUTERUTILS_H_ */

#endif /* USE_BOOST_LIBRARIES */
