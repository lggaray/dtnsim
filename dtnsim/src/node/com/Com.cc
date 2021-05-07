#include "src/node/com/Com.h"
#include "src/node/app/App.h"

Define_Module (Com);

void Com::initialize()
{
	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex();

	// Store packetLoss probability
	this->packetLoss_ = par("packetLoss").doubleValue();
}

void Com::setContactTopology(ContactPlan &contactTopology)
{
	this->contactTopology_ = contactTopology;
}

void Com::handleMessage(cMessage *msg)
{
	if (msg->getKind() == BUNDLE || msg->getKind() == BUNDLE_CUSTODY_REPORT)
	{
		BundlePkt* bundle = check_and_cast<BundlePkt *>(msg);

		if (eid_ == bundle->getNextHopEid())
		{
			// This is an inbound message, check if packet was lost on the way
			if (packetLoss_ > uniform(0, 1.0))
			{
				// Packet was lost in the way, delete it
				cout << simTime() << " Node " << eid_ << " Bundle id " << bundle->getBundleId() << " lost on the way!" << endl;
				delete bundle;
			}
			else
			{
				// received correctly, send to Dtn layer
				send(msg, "gateToDtn$o");
			}
		}
		else
		{
			// This is an outbound message, perform a delayed send
			cModule *destinationModule = this->getParentModule()->getParentModule()->getSubmodule("node", bundle->getNextHopEid())->getSubmodule("com");
			double linkDelay = contactTopology_.getRangeBySrcDst(eid_, bundle->getNextHopEid());
			if (linkDelay == -1)
			{
				cout << "warning, range not available for nodes " << eid_ << "-" << bundle->getNextHopEid() << ", assuming range is 0" << endl;
				linkDelay = 0;
			}

			sendDirect(msg, linkDelay, 0, destinationModule, "gateToAir");
		}
	}
}

void Com::finish()
{

}

Com::Com()
{

}

Com::~Com()
{

}

