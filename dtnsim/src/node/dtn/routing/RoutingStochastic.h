/*
 * According to Zhang's taxonomy which classifies routing protocols based on the availability
 * of knowledge about the network topology, We have two different kind of them:
 *
 * 	1) Deterministic:
 *	 					In this class, the schemes assume that information
 *						about the network topology is known a priori. The sub-categories in
 *						this class include tree-based, space and time based, and
 *						modification-based schemes.
 *
 * 	2) Stochastic:
 * 						In this class, the schemes assume that no prior information about the
 * 						node movement or network topology is available and thus, path selections
 * 						are based simply on heuristics, e.g., forwarding to neighbor nodes
 * 						(like epidemic routing), or based on the mobility pattern of nodes, or
 * 						link forwarding probability based.
 *
 * This class provides a implementation by default for inherited Routing's methods that result
 * suitable for the major stochastic schemes implemented here.
 * All routing methods which inherited from this class must implement routeAndQueueBundle method
 * since routing policies are routing scheme dependents.
 */

#ifndef SRC_NODE_DTN_ROUTING_ROUTINGSTOCHASTIC_H_
#define SRC_NODE_DTN_ROUTING_ROUTINGSTOCHASTIC_H_

#include <src/node/dtn/routing/Routing.h>

class RoutingStochastic: public Routing
{
public:
	RoutingStochastic(int eid, SdrModel * sdr, cModule * dtn);
	virtual ~RoutingStochastic();

	virtual void msgToOtherArrive(BundlePkt * bundle, double simTime);

	virtual bool msgToMeArrive(BundlePkt * bundle);

	virtual void contactStart(Contact *c);

	virtual void contactEnd(Contact *c);

	virtual void successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination);

	virtual void  refreshForwarding(Contact * c);

	// This is a pure virtual method (all stochastic routing must at least
	// implement this function)
	virtual void routeAndQueueBundle(Contact *c) = 0;


protected:
	cModule * dtn_;

	// The bundles this node has received as the final recipient or sent to final destination
	list<int> deliveredBundles_;

	bool isCarryingBundle(long bundleId);
	bool isDeliveredBundle(long bundleId);
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGSTOCHASTIC_H_ */
