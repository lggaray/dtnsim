#ifndef SRC_NODE_DTN_ROUTING_ROUTINGCGRMODELREV17_H_
#define SRC_NODE_DTN_ROUTING_ROUTINGCGRMODELREV17_H_

#include <src/node/dtn/routing/CgrRoute.h>
#include <src/node/dtn/routing/RoutingDeterministic.h>

#define	MAX_SPEED_MPH	(150000)

class RoutingCgrModelRev17: public RoutingDeterministic {
public:
	RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * localContactPlan,
			ContactPlan * globalContactPlan, string routingType, bool printDebug);
	virtual ~RoutingCgrModelRev17();
	virtual void routeAndQueueBundle(BundlePkt *bundle, double simTime);

	// stats recollection
	int getDijkstraCalls();
	int getDijkstraLoops();
	int getRouteTableEntriesCreated();
	int getRouteTableEntriesExplored();
	int getBundlesInLimbo();

	bool printDebug_ = true;

private:

	// Stats collection
	int dijkstraCalls;
	int dijkstraLoops;
	int tableEntriesCreated;
	int tableEntriesExplored;
	int bundlesInLimbo;

	// Basic variables
	string routingType_;
	int nodeNum_;
	double simTime_;

	void checkRoutingTypeString(void);

	// Route Table: one table per destination
	vector<vector<CgrRoute>> routeTable_;
	double routeTableLastEditTime = -1;

	typedef struct {
		Contact * predecessor;		// Predecessor Contact
		bool suppressed;			// Dijkstra exploration: suppressed
	} Work;

	void cgrForward(BundlePkt * bundle);
	void cgrEnqueue(BundlePkt * bundle, CgrRoute * bestRoute);

	void findNextBestRoute(vector<int> suppressedContactIds, int terminusNode, CgrRoute * route);

	void clearRouteTable();
	void printRouteTable(int terminusNode);
	static bool compareRoutes(CgrRoute i, CgrRoute j);
	void printContactPlan();
};

#endif /* SRC_NODE_DTN_ROUTING_ROUTINGCGRMODELREV17_H_ */
