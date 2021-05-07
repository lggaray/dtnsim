/*
 * RoutingSprayAndWait.h
 *
 *  Created on: Jul 18, 2017
 *      Author: FRaverta
 *
 * This class provides an implementation of Spray and Wait routing scheme proposed by T. Spyropoulos, K. Psounis and C. S. Raghavendra in
 * "Spray and Wait: An Efficient Routing Scheme for Intermittently Connected Mobile Networks, Proceedings of ACM SIG-COMM Workshop on
 *  Delay-Tolerant Networking (WDTN), 2005".
 *
 * This class provides classical and binary Spray And Wait algorithm implementations.
 *
 * Description of the Spray And Wait routing behavior
 *
 *	 Routing decisions are taken in two moments:
 * 			1) When a contact start
 *	 		2) When a bundle is successfully forwarded.
 *
 *	 In both moments, it enqueue a bundle to send according following criteria:
 *
 *		1) 	If there is a bundle whose destination is the contact destination, enqueue to send one copy of this.
 *
 * 		2)  If there aren't any bundle to that destination, enqueue a bundle with bundlesCopies > 1. Since if
 *			bundleCopy == 1 it must be delivered to destination only. If binary=false, it only sends one copy to next node. Otherwise it
 *			sends bundlesCopies/2.
 */

#ifndef SRC_NODE_DTN_ROUTINGSPRAYANDWAIT_H_
#define SRC_NODE_DTN_ROUTINGSPRAYANDWAIT_H_

#include <src/node/dtn/routing/RoutingStochastic.h>

class RoutingSprayAndWait : public RoutingStochastic
{
public:
	RoutingSprayAndWait(int eid, SdrModel * sdr, cModule * dtn, int amountOfCopies, bool binary);
	virtual ~RoutingSprayAndWait();
	virtual void msgToOtherArrive(BundlePkt * bundle, double simTime);
	virtual void contactEnd(Contact *c);
	virtual void successfulBundleForwarded(long bundleId, Contact * contact, bool sentToDestination);
	virtual void routeAndQueueBundle(Contact *c);

private:
	int amountOfCopies;
	bool binary;
};

#endif /* SRC_NODE_DTN_ROUTINGSPRAYANDWAIT_H_ */
