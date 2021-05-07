#include "src/node/dtn/routing/RoutingStochastic.h"

RoutingStochastic::RoutingStochastic(int eid, SdrModel * sdr, cModule * dtn)
	:Routing(eid,sdr)
{
	dtn_ = dtn;
}

RoutingStochastic::~RoutingStochastic()
{

}

void RoutingStochastic::msgToOtherArrive(BundlePkt * bundle, double simTime)
{
	//Check if message is already in bundles list, if it isn't, insert it.
	if(!isCarryingBundle(bundle->getBundleId()) and !isDeliveredBundle(bundle->getBundleId()))
		sdr_->enqueueBundle(bundle);
	else
		delete bundle;
}

bool RoutingStochastic::msgToMeArrive(BundlePkt * bundle)
{
	/*
	*Check if current bundle has already sent to app, if not
	*it tells to dtn that bundle should be delivered to app and
	*save it as delivered bundle.
	*/
	if (!isDeliveredBundle(bundle->getBundleId()))
	{
		deliveredBundles_.push_back(bundle->getBundleId());
		return true;
	}
	return false;
}

void RoutingStochastic::contactStart(Contact *c)
{
	//When a contact start, performs routing decisions.
	routeAndQueueBundle(c);
}

void RoutingStochastic::contactEnd(Contact *c)
{
	/*
	 * When a contact finishes, it takes bundles which
	 * weren't sent and removes these since we assume they are copies.
	 */
	while (sdr_->isBundleForContact(c->getId()))
	{
		BundlePkt* bundle = sdr_->getNextBundleForContact(c->getId());
		sdr_->popNextBundleForContact(c->getId());
		delete bundle;
	}
}

/***
 * Check if bundle successful forwarded is a delivered to its destination,
 * if that happens, it deletes this from bundles queue since there is no sense
 * in carrying a delivered to destination bundle.
 */
void RoutingStochastic::successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination)
{
	//Check if bundle was deliver to its final destination.
	//Note it is necessary to check if bundle is enqueue since bundle could be sent
	//to its final destination and deleted from queue while it was sending to another relay node.
	BundlePkt * bundle = sdr_->getEnqueuedBundle(bundleId);
	if( bundle != NULL && bundle->getDestinationEid() == contact->getDestinationEid()){
		sdr_->removeBundle(bundleId);
		deliveredBundles_.push_back(bundleId);
	}
}

void  RoutingStochastic::refreshForwarding(Contact * c)
{
	routeAndQueueBundle(c);
}

/**
 * Check if it is carrying a bundle with bundleId equals to bundleId.
 */
bool RoutingStochastic::isCarryingBundle(long bundleId)
{
	list<BundlePkt *> carryingBundles = sdr_->getCarryingBundles();
	for(list<BundlePkt *>::iterator it = carryingBundles.begin(); it != carryingBundles.end(); ++it){
		if( (*it)->getBundleId() == bundleId )
			return true;
	}
	return false;
}

/**
 * Check if bundleId has been delivered to App.
 */
bool RoutingStochastic::isDeliveredBundle(long bundleId)
{
	for(list<int>::iterator it = deliveredBundles_.begin(); it != deliveredBundles_.end(); ++it)
		if( (*it) == bundleId )
			return true;
	return false;
}




