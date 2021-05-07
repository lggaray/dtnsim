/*
 * RoutingCgrModelYen.h
 *
 *  Created on: Jan 24, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_DTN_ROUTINGCGRMODELYEN_H_
#define SRC_NODE_DTN_ROUTINGCGRMODELYEN_H_

#include <src/node/dtn/routing/RoutingDeterministic.h>
#include <src/node/dtn/SdrModel.h>

class RoutingCgrModelYen: public RoutingDeterministic
{
public:
	RoutingCgrModelYen(int eid, SdrModel * sdr, ContactPlan * contactPlan, bool printDebug);
	virtual ~RoutingCgrModelYen();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);
private:
	bool printDebug_ = true;

	/////////////////////////////////////////////////
	// Ion Cgr Functions based in libcgr.c (v 3.5.0):
	/////////////////////////////////////////////////
#define	MIN_CONFIDENCE_IMPROVEMENT	(.05)
#define MIN_NET_DELIVERY_CONFIDENCE	(.80)
#define MAX_XMIT_COPIES (20)
#define	MAX_SPEED_MPH	(150000)

	typedef struct
	{
		int toNodeNbr; 	//Initial-hop neighbor.
		double fromTime; 	// init tx time
		double toTime;	 	// Time at which route shuts down: earliest contact end time among all
		float arrivalConfidence;
		double arrivalTime;
		double maxCapacity; // in Bits
		vector<Contact *> hops; // list: IonCXref addr
		int rootPathLenght;
	} CgrRoute;

	typedef struct
	{
		int neighborNodeNbr;
		int contactId; // This is not, in ION
		double forfeitTime;
		double arrivalTime;
		float arrivalConfidence;
		unsigned int hopCount; // hops from dest. node.
		CgrRoute * route; // pointer to route so we can decrement capacities
		//Scalar	overbooked; 	//Bytes needing reforward.
		//Scalar	protected; 		//Bytes not overbooked.
	} ProximateNode;

	typedef struct
	{
		Contact * contact;
		Contact * predecessor;	// predecessor Contact
		double arrivalTime;
		double capacity; 	// in Bytes (?)
		bool visited;
		bool suppressed;
		vector<Contact *> suppressedNextContact;
	} Work;

	map<int, vector<CgrRoute> > routeList_;
	double routeListLastEditTime = -1;

	void cgrForward(BundlePkt * bundle, double simTime);
	void identifyProximateNodes(BundlePkt * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes);
	void loadRouteList(int terminusNode, double simTime);
	void loadRouteListYen(int terminusNode, double simTime);

	void findNextBestRoute(Contact * rootContact, int terminusNode, CgrRoute * route);
	void tryRoute(BundlePkt * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes);
	void recomputeRouteForContact();
	void enqueueToNeighbor(BundlePkt * bundle, ProximateNode * selectedNeighbor);
	void enqueueToLimbo(BundlePkt * bundle);
	void bpEnqueue(BundlePkt * bundle, ProximateNode * selectedNeighbor);
};

#endif /* SRC_NODE_DTN_ROUTINGCGRMODELYEN_H_ */
