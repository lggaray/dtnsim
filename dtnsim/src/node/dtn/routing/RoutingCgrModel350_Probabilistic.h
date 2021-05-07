
#ifndef SRC_NODE_DTN_ROUTINGCGRMODEL_PROBABILISTIC_H_
#define SRC_NODE_DTN_ROUTINGCGRMODEL_PROBABILISTIC_H_

#include <src/node/dtn/routing/CgrRoute.h>
#include <src/node/dtn/routing/RoutingDeterministic.h>
#include <src/node/dtn/SdrModel.h>

class RoutingCgrModel350_Probabilistic: public RoutingDeterministic
{
public:
	RoutingCgrModel350_Probabilistic(int eid, SdrModel * sdr, ContactPlan * contactPlan, bool printDebug, cModule * dtn, double sLinkProbability);
	virtual ~RoutingCgrModel350_Probabilistic();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);
	virtual CgrRoute* getCgrBestRoute(BundlePkt * bundle, double simTime);
	virtual vector<CgrRoute> getCgrRoutes(BundlePkt * bundle, double simTime);
	virtual bool msgToMeArrive(BundlePkt * bundle);
	virtual void contactStart(Contact *c);
	virtual void successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination);


	// stats recollection
	int getDijkstraCalls();
	int getDijkstraLoops();
	int getRouteTableEntriesExplored();

private:

	int dijkstraCalls;
	int dijkstraLoops;
	int tableEntriesExplored;

	//Successful link probability
	double sContactProb_;

	bool printDebug_ = false;
	// The bundles this node has received as the final recipient or sent to final destination
	list<int> deliveredBundles_;
	cModule * dtn_;


	/////////////////////////////////////////////////
	// Ion Cgr Functions based in libcgr.c (v 3.5.0):
	/////////////////////////////////////////////////
#define	MIN_CONFIDENCE_IMPROVEMENT	(.05)
#define MIN_DTN_DELIVERY_CONFIDENCE	(.80)
#define MAX_XMIT_COPIES (20)
#define	MAX_SPEED_MPH	(150000)

	struct ProximateNode
	{
		int neighborNodeNbr;
		int contactId; // This is not, in ION
		double forfeitTime;
		double arrivalTime;
		float confidence;
		unsigned int hopCount; // hops from dest. node.
		CgrRoute * route; // pointer to route so we can decrement capacities
		//Scalar	overbooked; 	//Bytes needing reforward.
		//Scalar	protected; 		//Bytes not overbooked.
	};

	typedef struct
	{
		Contact * contact;
		Contact * predecessor;	// predecessor Contact
		double arrivalTime;
		double capacity; 	// in Bytes
		bool visited;
		bool suppressed;
	} Work;

	map<int, vector<CgrRoute> > routeList_;
	double routeListLastEditTime = -1;
	enum Mode {hopCount,arrivalTime};

	ProximateNode* cgrForward(BundlePkt * bundle, double simTime, Mode mode);
	void identifyProximateNodes(BundlePkt * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes, Mode mode);
	void loadRouteList(int terminusNode, double simTime);

	void findNextBestRoute(Contact * rootContact, int terminusNode, CgrRoute * route);
	void tryRoute(BundlePkt * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes, Mode mode);
	void recomputeRouteForContact();
	void enqueueToNeighbor(BundlePkt * bundle, ProximateNode * selectedNeighbor);
	void enqueueToLimbo(BundlePkt * bundle);
	void bpEnqueue(BundlePkt * bundle, ProximateNode * selectedNeighbor);
	bool isDeliveredBundle(long bundleId);
};

#endif /* SRC_NODE_DTN_ROUTINGCGRMODEL_H_ */
