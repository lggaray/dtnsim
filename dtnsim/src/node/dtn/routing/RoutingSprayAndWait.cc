#include "src/node/dtn/routing/RoutingSprayAndWait.h"
#include "src/node/dtn/Dtn.h"

RoutingSprayAndWait::RoutingSprayAndWait(int eid, SdrModel * sdr, cModule * dtn, int amountOfCopies, bool binary)
	:RoutingStochastic(eid,sdr,dtn)
{
	this->binary = binary;
	this->amountOfCopies = amountOfCopies;
}

RoutingSprayAndWait::~RoutingSprayAndWait()
{
}

void RoutingSprayAndWait::msgToOtherArrive(BundlePkt * bundle, double simTime)
{
	if( bundle->getSourceEid() == eid_ )
		bundle->setBundlesCopies(amountOfCopies);

	RoutingStochastic::msgToOtherArrive(bundle,simTime);
}

void RoutingSprayAndWait::contactEnd(Contact *c)
{
	/** todo NO DEBERIA HABER MAS DE UN BUNDLE ENCOLADO!! Y DEBERIA REASIGNAR LAS COPIAS QUE NO SE ENVIARON A LA LISTA DE BUNDLES*/
	/*
	 * When a contact finishes, it takes bundles which
	 * weren't sent and removes these since we assume they are copies.
	 */
	if (sdr_->isBundleForContact(c->getId()))
	{
		BundlePkt* bundle = sdr_->getNextBundleForContact(c->getId());
		sdr_->popNextBundleForContact(c->getId());


		BundlePkt*  enqueueBundle = sdr_->getEnqueuedBundle(bundle->getBundleId());
		if( enqueueBundle != NULL)
			enqueueBundle->setBundlesCopies(enqueueBundle->getBundlesCopies() + bundle->getBundlesCopies());
		else
			cout<<"SprayAndWait::ContactEnd(Contact * c) - WARNNING: It try to set bundle's copies but bundle ID = "<< bundle->getBundleId() <<" was already dequeue."<<endl;
		delete bundle;

		//If there are more than one bundle report error and exit simulation. (For test propose)
		if(sdr_->isBundleForContact(c->getId()))
		{
			cout<<"SprayAndWait::ContactEnd(Contact * c) - More than one bundles is enqueue in a finished contact";
			exit(0);
		}
	}
}

/***
 * Check if bundle successful forwarded is a delivered to its destination,
 * if that happens, it deletes this from bundles queue since there is no sense
 * in carrying a delivered to destination bundle.
 */
void RoutingSprayAndWait::successfulBundleForwarded(long bundleId, Contact * contact,  bool sentToDestination)
{
	routeAndQueueBundle(contact);
	RoutingStochastic::successfulBundleForwarded(bundleId, contact, sentToDestination);

}




void RoutingSprayAndWait::routeAndQueueBundle(Contact *c)
{
	//Enqueue only one bundle per contact
	if(sdr_->isBundleForContact(c->getId()))
		return;

	RoutingSprayAndWait * other = check_and_cast<RoutingSprayAndWait *>(check_and_cast<Dtn *>(
																dtn_->getParentModule()->getParentModule()->getSubmodule("node", c->getDestinationEid())
																->getSubmodule("dtn"))->getRouting());

	list<BundlePkt *> carryingBundles = sdr_->getCarryingBundles();
	list<BundlePkt *> bundlesToOthers = list<BundlePkt *>();
	//Check if there are bundles whose destination is currentContact's destination
	//enqueue those bundles to this contact
	for(list<BundlePkt *>::iterator it = carryingBundles.begin(); it != carryingBundles.end(); ++it)
	{
		if((*it)->getDestinationEid() == c->getDestinationEid())
		{
			//Check if the other node has already received this bundle.
			//If it hasn't, send this.
			if ( !other->isDeliveredBundle((*it)->getBundleId()) )
			{
				(*it)->setBundlesCopies((*it)->getBundlesCopies() - 1);
				BundlePkt * bundleCopy = (*it)->dup();
				bundleCopy->setNextHopEid(c->getDestinationEid());
				bundleCopy->setBundlesCopies(1);
				sdr_->enqueueBundleToContact(bundleCopy, c->getId());
				return;
			}
			else
			{
				//since bundle has already reached its final destination, delete it and add its id to deliveredBundles_.
				deliveredBundles_.push_back((*it)->getBundleId());
				sdr_->removeBundle((*it)->getBundleId());
			}
		}
		else
			if((*it)->getBundlesCopies() > 1)
				bundlesToOthers.push_back(*it);
	}

	//Enqueue others bundles to currentContact
	for(list<BundlePkt *>::iterator it = bundlesToOthers.begin(); it != bundlesToOthers.end(); it++)
	{
		//Check if the other node has already received this bundle.
		//If it hasn't, send this.
		if ( !other->isCarryingBundle((*it)->getBundleId()) ){
			int copies = (*it)->getBundlesCopies();

			BundlePkt * bundleCopy = (*it)->dup();
			bundleCopy->setNextHopEid(c->getDestinationEid());
			bundleCopy->setBundlesCopies( (binary)? copies / 2 : 1 );
			sdr_->enqueueBundleToContact(bundleCopy, c->getId());

			(*it)->setBundlesCopies( (binary)? copies / 2 +  copies % 2 : copies - 1 );
			return;
		}
	}
}




