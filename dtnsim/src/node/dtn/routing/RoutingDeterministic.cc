#include "src/node/dtn/routing/RoutingDeterministic.h"

RoutingDeterministic::RoutingDeterministic(int eid, SdrModel * sdr, ContactPlan * contactPlan)
	:Routing(eid,sdr)
{
	contactPlan_ = contactPlan;
}

RoutingDeterministic::~RoutingDeterministic()
{
}

void RoutingDeterministic::msgToOtherArrive(BundlePkt * bundle, double simTime)
{
	// Route and enqueue bundle
	routeAndQueueBundle(bundle, simTime);
}

bool RoutingDeterministic::msgToMeArrive(BundlePkt * bundle)
{
	return true;
}

void RoutingDeterministic::contactStart(Contact *c)
{

}

void RoutingDeterministic::successfulBundleForwarded(long bundleId, Contact * c,  bool sentToDestination)
{

}

void  RoutingDeterministic::refreshForwarding(Contact * c)
{

}

void RoutingDeterministic::contactEnd(Contact *c)
{
	while (sdr_->isBundleForContact(c->getId()))
	{
		BundlePkt* bundle = sdr_->getNextBundleForContact(c->getId());
		sdr_->popNextBundleForContact(c->getId());

		//emit(dtnBundleReRouted, true);
		routeAndQueueBundle(bundle, simTime().dbl());
	}
}




