#include "src/node/dtn/routing/RoutingDirect.h"

RoutingDirect::RoutingDirect(int eid, SdrModel * sdr, ContactPlan * contactPlan)
	: RoutingDeterministic(eid, sdr, contactPlan)
{
}

RoutingDirect::~RoutingDirect()
{

}

void RoutingDirect::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	int contactId=0; // contact 0 is the limbo

	// Search for the target contact to send the bundle
	int neighborEid = bundle->getDestinationEid();
	double earliestStart = numeric_limits<double>::max();

	vector<Contact> contacts = contactPlan_->getContactsBySrcDst(eid_,neighborEid);
	for(size_t i = 0; i<contacts.size(); i++){
		if((contacts.at(i).getEnd()>simTime)&&(contacts.at(i).getStart()<earliestStart))
		{
			contactId=contacts.at(i).getId();
			earliestStart = contacts.at(i).getStart();
		}
	}

	// Enqueue the bundle
	if(contactId != 0)
	{
		bundle->setNextHopEid(contactPlan_->getContactById(contactId)->getDestinationEid());
	}
	sdr_->enqueueBundleToContact(bundle, contactId);
}


