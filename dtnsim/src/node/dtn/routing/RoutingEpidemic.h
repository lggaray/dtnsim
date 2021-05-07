/*
 * RoutingEpidemic.h
 *
 *  Created on: Jul 18, 2017
 *      Author: FRaverta
 *
 * This class provides a simplified implementation of Epidemic routing scheme proposed by Vahdat and Becker in
 * "Epidemic routing for partially connected ad-hoc networks, Technical Report CS-200006. Duke University; 2000".
 *
 * Description of the Epidemic routing behavior:
 *
 * 		1) When a contact starts it enqueue to send all bundles whose destination is the contact's destination if target node has not received it.
 * 		   Here we assume that there isn't any cost associated to get this information.
 * 		   This bundles are deleted from bundles_ list.
 *
 * 		2) Enqueue to send those bundles, intended to other nodes, which has not been received by the other yet.
 * 		   Again, we assume that there isn't any cost associated to get this information.
 * 		   This bundles aren't deleted from bundles_ list, since these will be sent again in the next contact opportunity.
 */

#ifndef SRC_NODE_DTN_ROUTINGEPIDEMIC_H_
#define SRC_NODE_DTN_ROUTINGEPIDEMIC_H_

#include <src/node/dtn/routing/RoutingStochastic.h>

class RoutingEpidemic : public RoutingStochastic
{
public:
	RoutingEpidemic(int eid, SdrModel * sdr, cModule * dtn);
	virtual ~RoutingEpidemic();
	virtual void routeAndQueueBundle(Contact *c);

};

#endif /* SRC_NODE_DTN_ROUTINGEPIDEMIC_H_ */
