/*
 * Routing.h
 *
 *  Created on: July 13, 2017
 *      Author: fraverta
 *
 * This file declares an interface that any routing method must implement.
 *
 */

#ifndef SRC_NODE_NET_ROUTING_H_
#define SRC_NODE_NET_ROUTING_H_

#include <src/node/dtn/ContactPlan.h>
#include <src/node/dtn/SdrModel.h>
#include <map>
#include <queue>
#include <limits>
#include <algorithm>
#include "src/dtnsim_m.h"

using namespace omnetpp;
using namespace std;

class Routing
{
public:
	Routing(int eid, SdrModel * sdr)
	{
		eid_ = eid;
		sdr_ = sdr;
	}
	virtual ~Routing()
	{
	}

	/**
	 * Method that will be called by Dtn module when a message to other destination in the
	 * network  arrives.
	 */
	virtual void msgToOtherArrive(BundlePkt * bundle, double simTime) = 0;

	/**
	 * Method that will be called by Dtn module when a message to this node arrives.
	 *
	 * @Return - true if bundle must be send to app layer.
	 */
	virtual bool msgToMeArrive(BundlePkt * bundle) = 0;

	/**
	 * Method that will be called by Dtn module when a contact starts.
	 */
	virtual void contactStart(Contact *c) = 0;

	/**
	 * Method that will be called by Dtn module when a contact ends.
	 */
	virtual void contactEnd(Contact *c) = 0;

	virtual void  refreshForwarding(Contact * c) = 0;

	/**
	 * Method that will be called by Dtn module when some bundle is forwarded successfully
	 */
	virtual void successfulBundleForwarded(long bundleId, Contact * contact, bool sentToDestination)=0;


protected:
	//Endpoint id
	int eid_;

	//Sdr model to enqueue bundles for transmission
	SdrModel * sdr_;
};

#endif /* SRC_NODE_NET_ROUTING_H_ */
