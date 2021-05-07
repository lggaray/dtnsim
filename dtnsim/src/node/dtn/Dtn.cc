#include "src/node/dtn/Dtn.h"
#include "src/node/app/App.h"

Define_Module (Dtn);

Dtn::Dtn()
{

}

Dtn::~Dtn()
{

}

void Dtn::setContactPlan(ContactPlan &contactPlan)
{
	this->contactPlan_ = contactPlan;
}

void Dtn::setContactTopology(ContactPlan &contactTopology)
{
	this->contactTopology_ = contactTopology;
}

int Dtn::numInitStages() const
{
	int stages = 2;
	return stages;
}

void Dtn::initialize(int stage)
{
	if (stage == 1)
	{
		// Store this node eid
		this->eid_ = this->getParentModule()->getIndex();

		this->custodyTimeout_ = par("custodyTimeout");
		this->custodyModel_.setEid(eid_);
		this->custodyModel_.setSdr(&sdr_);
		this->custodyModel_.setCustodyReportByteSize(par("custodyReportByteSize"));

		// Get a pointer to graphics module
		graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");
		// Register this object as sdr obsever, in order to display bundles stored in sdr properly.
		sdr_.addObserver(this);
		update();

		// Schedule local starts contact messages.
		// Only contactTopology starts contacts are scheduled.
		vector<Contact> localContacts1 = contactTopology_.getContactsBySrc(this->eid_);
		for (vector<Contact>::iterator it = localContacts1.begin(); it != localContacts1.end(); ++it)
		{
			ContactMsg *contactMsgStart = new ContactMsg("contactStart", CONTACT_START_TIMER);

			contactMsgStart->setSchedulingPriority(CONTACT_START_TIMER);
			contactMsgStart->setId((*it).getId());
			contactMsgStart->setStart((*it).getStart());
			contactMsgStart->setEnd((*it).getEnd());
			contactMsgStart->setDuration((*it).getEnd() - (*it).getStart());
			contactMsgStart->setSourceEid((*it).getSourceEid());
			contactMsgStart->setDestinationEid((*it).getDestinationEid());
			contactMsgStart->setDataRate((*it).getDataRate());

			scheduleAt((*it).getStart(), contactMsgStart);

			EV << "node " << eid_ << ": " << "a contact +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getDataRate() << endl;
		}
		// Schedule local ends contact messages.
		// All ends contacts of the contactPlan are scheduled.
		// to trigger re-routings of bundles queued in contacts that did not happen.
		vector<Contact> localContacts2 = contactPlan_.getContactsBySrc(this->eid_);
		for (vector<Contact>::iterator it = localContacts2.begin(); it != localContacts2.end(); ++it)
		{
			ContactMsg *contactMsgEnd = new ContactMsg("contactEnd", CONTACT_END_TIMER);

			contactMsgEnd->setName("ContactEnd");
			contactMsgEnd->setSchedulingPriority(CONTACT_END_TIMER);
			contactMsgEnd->setId((*it).getId());
			contactMsgEnd->setStart((*it).getStart());
			contactMsgEnd->setEnd((*it).getEnd());
			contactMsgEnd->setDuration((*it).getEnd() - (*it).getStart());
			contactMsgEnd->setSourceEid((*it).getSourceEid());
			contactMsgEnd->setDestinationEid((*it).getDestinationEid());
			contactMsgEnd->setDataRate((*it).getDataRate());

			scheduleAt((*it).getStart() + (*it).getDuration(), contactMsgEnd);
		}

		// Initialize routing
		this->sdr_.setEid(eid_);
		this->sdr_.setSize(par("sdrSize"));
		this->sdr_.setNodesNumber(this->getParentModule()->getParentModule()->par("nodesNumber"));
		this->sdr_.setContactPlan(&contactPlan_);

		string routeString = par("routing");

		if (routeString.compare("direct") == 0)
			routing = new RoutingDirect(eid_, &sdr_, &contactPlan_);
		else if (routeString.compare("cgrModel350") == 0)
			routing = new RoutingCgrModel350(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"));
		else if (routeString.compare("cgrModel350_Hops") == 0)
			routing = new RoutingCgrModel350_Hops(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"));
		else if (routeString.compare("cgrModelYen") == 0)
			routing = new RoutingCgrModelYen(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"));
		else if (routeString.compare("cgrModelRev17") == 0)
		{
			ContactPlan * globalContactPlan = ((Dtn *) this->getParentModule()->getParentModule()->getSubmodule("node", 0)->getSubmodule("dtn"))->getContactPlanPointer();
			routing = new RoutingCgrModelRev17(eid_, this->getParentModule()->getVectorSize(), &sdr_, &contactPlan_, globalContactPlan, par("routingType"), par("printRoutingDebug"));
		}
		else if (routeString.compare("epidemic") == 0)
		{
			routing = new RoutingEpidemic(eid_, &sdr_, this);
		}
		else if (routeString.compare("sprayAndWait") == 0)
		{
			int bundlesCopies = par("bundlesCopies");
			routing = new RoutingSprayAndWait(eid_, &sdr_, this, bundlesCopies, false);
		}
		else if (routeString.compare("binarySprayAndWait") == 0)
		{
			int bundlesCopies = par("bundlesCopies");
			routing = new RoutingSprayAndWait(eid_, &sdr_, this, bundlesCopies, true);
		}
		else if (routeString.compare("cgrModel350_2Copies") == 0)
			routing = new RoutingCgrModel350_2Copies(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"), this);
		else if (routeString.compare("cgrModel350_Probabilistic") == 0)
		{
			double sContactProb = par("sContactProb");
			routing = new RoutingCgrModel350_Probabilistic(eid_, &sdr_, &contactPlan_, par("printRoutingDebug"), this, sContactProb);
		}
		else
		{
			cout << "dtnsim error: unknown routing type: " << routeString << endl;
			exit(1);
		}

		// Register signals
		dtnBundleSentToCom = registerSignal("dtnBundleSentToCom");
		dtnBundleSentToApp = registerSignal("dtnBundleSentToApp");
		dtnBundleSentToAppHopCount = registerSignal("dtnBundleSentToAppHopCount");
		dtnBundleSentToAppRevisitedHops = registerSignal("dtnBundleSentToAppRevisitedHops");
		dtnBundleReceivedFromCom = registerSignal("dtnBundleReceivedFromCom");
		dtnBundleReceivedFromApp = registerSignal("dtnBundleReceivedFromApp");
		dtnBundleReRouted = registerSignal("dtnBundleReRouted");
		sdrBundleStored = registerSignal("sdrBundleStored");
		sdrBytesStored = registerSignal("sdrBytesStored");
		routeCgrDijkstraCalls = registerSignal("routeCgrDijkstraCalls");
		routeCgrDijkstraLoops = registerSignal("routeCgrDijkstraLoops");
		routeCgrRouteTableEntriesCreated = registerSignal("routeCgrRouteTableEntriesCreated");
		routeCgrRouteTableEntriesExplored = registerSignal("routeCgrRouteTableEntriesExplored");
		dtnCgrCalls = registerSignal("dtnCgrCalls");
		sdrBundlesDeleted = registerSignal("sdrBundlesDeleted");
		routeBundlesInLimbo = registerSignal("routeBundlesInLimbo");

		if (eid_ != 0)
		{
			emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
			emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
		}

		// Initialize BundleMap
		this->saveBundleMap_ = par("saveBundleMap");
		if (saveBundleMap_ && eid_ != 0)
		{
			// create result folder if it doesn't exist
			struct stat st =
			{ 0 };
			if (stat("results", &st) == -1)
			{
				mkdir("results", 0700);
			}

			string fileStr = "results/BundleMap_Node" + to_string(eid_) + ".csv";
			bundleMap_.open(fileStr);
			bundleMap_ << "SimTime" << "," << "SRC" << "," << "DST" << "," << "TSRC" << "," << "TDST" << "," << "BitLenght" << "," << "DurationSec" << endl;
		}
	}
}

void Dtn::finish()
{
	// Last call to sample-hold type metrics
	if (eid_ != 0)
	{
		emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
		emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
	}

	// Delete scheduled forwardingMsg
	std::map<int, ForwardingMsgStart *>::iterator it;
	for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
	{
		ForwardingMsgStart * forwardingMsg = it->second;
		cancelAndDelete(forwardingMsg);
	}

	// Delete all stored bundles
	sdr_.freeSdr(eid_);

	// BundleMap End
	if (saveBundleMap_)
		bundleMap_.close();

	delete routing;
}

void Dtn::handleMessage(cMessage * msg)
{
	///////////////////////////////////////////
	// New Bundle (from App or Com):
	///////////////////////////////////////////
	if (msg->getKind() == BUNDLE || msg->getKind() == BUNDLE_CUSTODY_REPORT)
	{
		if (msg->arrivedOn("gateToCom$i"))
			emit(dtnBundleReceivedFromCom, true);
		if (msg->arrivedOn("gateToApp$i"))
			emit(dtnBundleReceivedFromApp, true);

		BundlePkt* bundle = check_and_cast<BundlePkt *>(msg);
		dispatchBundle(bundle);
	}

	///////////////////////////////////////////
	// Contact Start and End
	///////////////////////////////////////////
	else if (msg->getKind() == CONTACT_START_TIMER)
	{
		// Schedule end of contact
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);

		// Visualize contact line on
		graphicsModule->setContactOn(contactMsg);

		// Call to routing algorithm
		routing->contactStart(contactPlan_.getContactById(contactMsg->getId()));

		// Schedule start of transmission
		ForwardingMsgStart* forwardingMsg = new ForwardingMsgStart("forwardingMsgStart", FORWARDING_MSG_START);
		forwardingMsg->setSchedulingPriority(FORWARDING_MSG_START);
		forwardingMsg->setNeighborEid(contactMsg->getDestinationEid());
		forwardingMsg->setContactId(contactMsg->getId());
		forwardingMsgs_[contactMsg->getId()] = forwardingMsg;
		scheduleAt(simTime(), forwardingMsg);

		delete contactMsg;
	}
	else if (msg->getKind() == CONTACT_END_TIMER)
	{
		// Finish transmission: If bundles are left in contact re-route them
		ContactMsg* contactMsg = check_and_cast<ContactMsg *>(msg);

		for (int i = 0; i < sdr_.getBundlesCountInContact(contactMsg->getId()); i++)
			emit(dtnBundleReRouted, true);

		routing->contactEnd(contactPlan_.getContactById(contactMsg->getId()));

		// Visualize contact line off
		graphicsModule->setContactOff(contactMsg);

		// Delete contactMsg
		cancelAndDelete(forwardingMsgs_[contactMsg->getId()]);
		forwardingMsgs_.erase(contactMsg->getId());
		delete contactMsg;
	}

	///////////////////////////////////////////
	// Forwarding Stage
	///////////////////////////////////////////
	else if (msg->getKind() == FORWARDING_MSG_START)
	{
		ForwardingMsgStart* forwardingMsgStart = check_and_cast<ForwardingMsgStart *>(msg);
		int neighborEid = forwardingMsgStart->getNeighborEid();
		int contactId = forwardingMsgStart->getContactId();

		// save freeChannelMsg to cancel event if necessary
		forwardingMsgs_[forwardingMsgStart->getContactId()] = forwardingMsgStart;

		// if there are messages in the queue for this contact
		if (sdr_.isBundleForContact(contactId))
		{
			// If local/remote node are responsive, then transmit bundle
			Dtn * neighborDtn = check_and_cast<Dtn *>(this->getParentModule()->getParentModule()->getSubmodule("node", neighborEid)->getSubmodule("dtn"));
			if ((!neighborDtn->onFault) && (!this->onFault))
			{
				// Get bundle pointer from sdr
				BundlePkt* bundle = sdr_.getNextBundleForContact(contactId);

				// Calculate data rate and Tx duration
				double dataRate = contactTopology_.getContactById(contactId)->getDataRate();
				double txDuration = (double) bundle->getByteLength() / dataRate;
				double linkDelay = contactTopology_.getRangeBySrcDst(eid_, neighborEid);

				Contact * contact = contactTopology_.getContactById(contactId);

				// if the message can be fully transmitted before the end of the contact, transmit it
				if ((simTime() + txDuration + linkDelay) <= contact->getEnd())
				{
					// Set bundle metadata (set by intermediate nodes)
					bundle->setSenderEid(eid_);
					bundle->setHopCount(bundle->getHopCount() + 1);
					bundle->getVisitedNodes().push_back(eid_);
					bundle->setXmitCopiesCount(0);

					//cout<<"-----> sending bundle to node "<<bundle->getNextHopEid()<<endl;
					send(bundle, "gateToCom$o");

					if (saveBundleMap_)
						bundleMap_ << simTime() << "," << eid_ << "," << neighborEid << "," << bundle->getSourceEid() << "," << bundle->getDestinationEid() << "," << bundle->getBitLength() << "," << txDuration << endl;

					sdr_.popNextBundleForContact(contactId);

					// If custody requested, store a copy of the bundle until report received
					if (bundle->getCustodyTransferRequested())
					{
						sdr_.enqueueTransmittedBundleInCustody(bundle->dup());
						this->custodyModel_.printBundlesInCustody();

						// Enqueue a retransmission event in case custody acceptance not received
						CustodyTimout * custodyTimeout = new CustodyTimout("custodyTimeout", CUSTODY_TIMEOUT);
						custodyTimeout->setBundleId(bundle->getBundleId());
						scheduleAt(simTime() + this->custodyTimeout_, custodyTimeout);
					}

					emit(dtnBundleSentToCom, true);
					emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
					emit(sdrBytesStored, sdr_.getBytesStoredInSdr());

					// Schedule next transmission
					scheduleAt(simTime() + txDuration, forwardingMsgStart);

					// Schedule forwarding message end
					ForwardingMsgEnd * forwardingMsgEnd = new ForwardingMsgEnd("forwardingMsgEnd", FORWARDING_MSG_END);
					forwardingMsgEnd->setSchedulingPriority(FORWARDING_MSG_END);
					forwardingMsgEnd->setNeighborEid(neighborEid);
					forwardingMsgEnd->setContactId(contactId);
					forwardingMsgEnd->setBundleId(bundle->getBundleId());
					forwardingMsgEnd->setSentToDestination(neighborEid == bundle->getDestinationEid());
					scheduleAt(simTime() + txDuration, forwardingMsgEnd);
				}
			}
			else
			{
				// If local/remote node unresponsive, then do nothing.
				// fault recovery will trigger a local and remote refreshForwarding
			}
		}
		else
		{
			// There are no messages in the queue for this contact
			// Do nothing, if new data arrives, a refreshForwarding
			// will wake up this forwarding thread
		}
	}
	else if (msg->getKind() == FORWARDING_MSG_END)
	{
		// A bundle was successfully forwarded. Notify routing schema in order to it makes proper decisions.
		ForwardingMsgEnd* forwardingMsgEnd = check_and_cast<ForwardingMsgEnd *>(msg);
		int bundleId = forwardingMsgEnd->getBundleId();
		int contactId = forwardingMsgEnd->getContactId();
		Contact * contact = contactTopology_.getContactById(contactId);

		routing->successfulBundleForwarded(bundleId, contact, forwardingMsgEnd->getSentToDestination());
		delete forwardingMsgEnd;
	}
	///////////////////////////////////////////
	// Custody retransmission timer
	///////////////////////////////////////////
	else if (msg->getKind() == CUSTODY_TIMEOUT)
	{
		// Custody timer expired, check if bundle still in custody memory space and retransmit it if positive
		CustodyTimout* custodyTimout = check_and_cast<CustodyTimout *>(msg);
		BundlePkt * reSendBundle = this->custodyModel_.custodyTimerExpired(custodyTimout);

		if(reSendBundle!=NULL)
			this->dispatchBundle(reSendBundle);

		delete custodyTimout;
	}
}

void Dtn::dispatchBundle(BundlePkt *bundle)
{
	//char bundleName[10];
	//sprintf(bundleName, "Src:%d,Dst:%d(id:%d)", bundle->getSourceEid() , bundle->getDestinationEid(), (int) bundle->getId());
	//cout << "Dispatching " << bundleName << endl;

	//emit(routeBundlesInLimbo, ((RoutingCgrModelRev17*) routing)->getBundlesInLimbo());

	if (this->eid_ == bundle->getDestinationEid())
	{
		// We are the final destination of this bundle
		emit(dtnBundleSentToApp, true);
		emit(dtnBundleSentToAppHopCount, bundle->getHopCount());
		bundle->getVisitedNodes().sort();
		bundle->getVisitedNodes().unique();
		emit(dtnBundleSentToAppRevisitedHops, bundle->getHopCount() - bundle->getVisitedNodes().size());

		// Check if this bundle has previously arrived here
		if (routing->msgToMeArrive(bundle))
		{
			// This is the first time this bundle arrives
			if (bundle->getBundleIsCustodyReport())
			{
				// This is a custody report destined to me
				BundlePkt * reSendBundle = this->custodyModel_.custodyReportArrived(bundle);

				// If custody was rejected, reroute
				if(reSendBundle!=NULL)
					this->dispatchBundle(reSendBundle);
			}
			else
			{
				// This is a data bundle destined to me
				if (bundle->getCustodyTransferRequested())
					this->dispatchBundle(this->custodyModel_.bundleWithCustodyRequestedArrived(bundle));

				// Send to app layer
				send(bundle, "gateToApp$o");
			}
		}
		else
			// A copy of this bundle was previously received
			delete bundle;
	}
	else
	{
		// This is a bundle in transit

		// Manage custody transfer
		if (bundle->getCustodyTransferRequested())
			this->dispatchBundle(this->custodyModel_.bundleWithCustodyRequestedArrived(bundle));

		// Either accepted or rejected custody, route bundle
		routing->msgToOtherArrive(bundle, simTime().dbl());

		// Emit routing specific statistics
		string routeString = par("routing");
		if (routeString.compare("cgrModel350") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModel350*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModel350*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModel350*) routing)->getRouteTableEntriesExplored());
		}
		if (routeString.compare("cgrModel350_3") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModel350_Hops*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModel350_Hops*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModel350_Hops*) routing)->getRouteTableEntriesExplored());
		}
		if (routeString.compare("cgrModelRev17") == 0)
		{
			emit(routeCgrDijkstraCalls, ((RoutingCgrModelRev17*) routing)->getDijkstraCalls());
			emit(routeCgrDijkstraLoops, ((RoutingCgrModelRev17*) routing)->getDijkstraLoops());
			emit(routeCgrRouteTableEntriesCreated, ((RoutingCgrModelRev17*) routing)->getRouteTableEntriesCreated());
			emit(routeCgrRouteTableEntriesExplored, ((RoutingCgrModelRev17*) routing)->getRouteTableEntriesExplored());
		}
		emit(sdrBundleStored, sdr_.getBundlesCountInSdr());
		emit(sdrBytesStored, sdr_.getBytesStoredInSdr());
		emit(dtnCgrCalls, 1);
		emit(sdrBundlesDeleted, sdr_.getBundlesDeletedInSdr());
		emit(routeBundlesInLimbo, ((RoutingCgrModelRev17*) routing)->getBundlesInLimbo());

		// Wake-up sleeping forwarding threads
		this->refreshForwarding();
	}
}

void Dtn::refreshForwarding()
{
    // Check all on-going forwardingMsgs threads
    // (contacts) and wake up those not scheduled.
    std::map<int, ForwardingMsgStart *>::iterator it;
    for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
    {
        ForwardingMsgStart * forwardingMsg = it->second;
        int cid = forwardingMsg->getContactId();
        if(!sdr_.isBundleForContact(cid))
        	//notify routing protocol that it has messages to send and contacts for routing
                routing->refreshForwarding(contactPlan_.getContactById(cid));
        if (!forwardingMsg->isScheduled()){
            scheduleAt(simTime(), forwardingMsg);
        }
    }
}

void Dtn::setOnFault(bool onFault)
{
	this->onFault = onFault;

	// Local and remote forwarding recovery
	if (onFault == false)
	{
		// Wake-up local un-scheduled forwarding threads
		this->refreshForwarding();

		// Wake-up remote un-scheduled forwarding threads
		std::map<int, ForwardingMsgStart *>::iterator it;
		for (it = forwardingMsgs_.begin(); it != forwardingMsgs_.end(); ++it)
		{
			ForwardingMsgStart * forwardingMsg = it->second;
			Dtn * remoteDtn = (Dtn *) this->getParentModule()->getParentModule()->getSubmodule("node", forwardingMsg->getNeighborEid())->getSubmodule("dtn");
			remoteDtn->refreshForwarding();
		}
	}
}

ContactPlan* Dtn::getContactPlanPointer(void)
{
	return &this->contactPlan_;
}

Routing * Dtn::getRouting(void)
{
	return this->routing;
}

/**
 * Implementation of method inherited from observer to update gui according to the number of
 * bundles stored in sdr.
 */
void Dtn::update(void)
{
	// update srd size text
	graphicsModule->setBundlesInSdr(sdr_.getBundlesCountInSdr());
}

