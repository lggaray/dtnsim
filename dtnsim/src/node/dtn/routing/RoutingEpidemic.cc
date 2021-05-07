#include "src/node/dtn/routing/RoutingEpidemic.h"
#include "src/node/dtn/Dtn.h"

RoutingEpidemic::RoutingEpidemic(int eid, SdrModel * sdr, cModule * dtn)
	:RoutingStochastic(eid,sdr,dtn)
{
}

RoutingEpidemic::~RoutingEpidemic()
{
}

void RoutingEpidemic::routeAndQueueBundle(Contact *c)
{
	//Enqueue only one bundle per contact
	if(sdr_->isBundleForContact(c->getId()))
		return;

	RoutingEpidemic * other = check_and_cast<RoutingEpidemic *>(check_and_cast<Dtn *>(
																dtn_->getParentModule()->getParentModule()->getSubmodule("node", c->getDestinationEid())
																->getSubmodule("dtn"))->getRouting());

	list<BundlePkt *> carryingBundles = sdr_->getCarryingBundles();
	list<BundlePkt *> bundlesToOthers = list<BundlePkt *>();
	//Check if there are bundles whose destination is currentContact's destination
	//enqueue those bundles to this contact
	for(list<BundlePkt *>::iterator it = carryingBundles.begin(); it != carryingBundles.end(); it++)
	{
		if((*it)->getDestinationEid() == c->getDestinationEid())
		{
			//Check if the other node has already received this bundle.
			//If it hasn't, send this.
			if ( !other->isDeliveredBundle((*it)->getBundleId()) )
			{
				BundlePkt * bundleCopy = (*it)->dup();
				bundleCopy->setNextHopEid(c->getDestinationEid());
				sdr_->enqueueBundleToContact(bundleCopy, c->getId());
			}
			else
			{
				//since bundle has already reached its final destination, delete it and add its id to deliveredBundles_.
				deliveredBundles_.push_back((*it)->getBundleId());
				sdr_->removeBundle((*it)->getBundleId());
			}
		}
		else
			bundlesToOthers.push_back(*it);
	}

	//Enqueue others bundles to currentContact
	for(list<BundlePkt *>::iterator it = bundlesToOthers.begin(); it != bundlesToOthers.end(); it++)
	{
		//Check if the other node has already received this bundle.
		//If it hasn't, send this.
		if ( !other->isCarryingBundle((*it)->getBundleId()) ){
			BundlePkt * bundleCopy = (*it)->dup();
			bundleCopy->setNextHopEid(c->getDestinationEid());
			sdr_->enqueueBundleToContact(bundleCopy, c->getId());
		}
	}
}




