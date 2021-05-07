#include "RoutingCgrModel350_Hops.h"

RoutingCgrModel350_Hops::RoutingCgrModel350_Hops(int eid, SdrModel * sdr, ContactPlan * contactPlan, bool printDebug) :
RoutingDeterministic(eid, sdr, contactPlan)
{
	eid_ = eid;
	sdr_ = sdr;
	contactPlan_ = contactPlan;
	printDebug_ = printDebug;
}

RoutingCgrModel350_Hops::~RoutingCgrModel350_Hops()
{

}

void RoutingCgrModel350_Hops::routeAndQueueBundle(BundlePkt * bundle, double simTime)
{
	if (!printDebug_) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

	// Reset counters
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	tableEntriesExplored = 0;
	changedRoutes = 0;

	// Call cgrForward from ion (route and forwarding)
	cgrForward(bundle, simTime);

	if (!printDebug_)
		cout.clear();

}

/////////////////////////////////////////////////
// Ion Cgr Functions based in libcgr.c (v 3.5.0):
/////////////////////////////////////////////////

void RoutingCgrModel350_Hops::cgrForward(BundlePkt * bundle, double simTime)
{
	cout << "TIME: " << simTime << "s, NODE: " << eid_ << ", routing bundle to dst: " << bundle->getDestinationEid() << " (" << bundle->getByteLength() << "Bytes)" << endl;

	// If contact plan was changed, discard route list
	if (contactPlan_->getLastEditTime() > routeListLastEditTime)
		routeList_.clear();

	vector<ProximateNode> proximateNodes;
	vector<int> excludedNodes;

	// Insert sender node to excludedNodes to avoid loops
	if (bundle->getReturnToSender() == false)
		excludedNodes.push_back(bundle->getSenderEid());

	// Insert other nodes into excludedNodes (embargoed nodes)
	// Not happening in the DtnSim

	// Populate proximateNodes
	// cout << "  calling identifyProximateNodes: " << endl;
	identifyProximateNodes(bundle, simTime, excludedNodes, &proximateNodes);

	// TODO: send critical bundle to all proximateNodes
	if (bundle->getCritical())
	{
		cout << "***Critical bundle forwarding not implemented yet!***" << endl;
		exit(1);
	}

	//cout << "  proximateNodesSize: " << proximateNodes.size() << ":" << endl;

	ProximateNode * selectedNeighbor = NULL;
	for (vector<ProximateNode>::iterator it = proximateNodes.begin(); it != proximateNodes.end(); ++it)
	{
		cout << "routeTable[" << bundle->getDestinationEid() << "][" << (*it).neighborNodeNbr << "]: nextHop: " << (*it).neighborNodeNbr << " (cId:" << (*it).contactId << ", resCap:"
				<< contactPlan_->getContactById((*it).contactId)->getResidualVolume() << "Bytes) arrivalConf:" << (*it).confidence << " arrivalT:" << (*it).arrivalTime << " hopCnt:" << (*it).hopCount << " forfT:" << (*it).forfeitTime
				<< endl;
		//cout << "routeTable[" << terminusNode << "][" << (*it).neighborNodeNbr << "]: nextHop: " << (*it).neighborNodeNbr << ", frm " << route.fromTime << " to " << (*it).arrivalTime << ", arrival time: " << route.arrivalTime << ", volume: " << route.residualVolume << "/" << route.maxVolume << "Bytes" << endl;

		// If the confidence improvement is less than the minimal, continue
		if (bundle->getDlvConfidence() > 0.0 && bundle->getDlvConfidence() < 1.0)
		{
			float newDlvConfidence = 1.0 - (1.0 - bundle->getDlvConfidence()) * (1.0 - it->confidence);
			float confidenceImprovement = (newDlvConfidence / bundle->getDlvConfidence()) - 1.0;
			if (confidenceImprovement < MIN_CONFIDENCE_IMPROVEMENT)
			{
				//cout << " Not chosen, not enough confidence improvement" << endl;
				continue;
			}
		}

		// Select Neighbor by arrival confidence, arrival time, hop count, and node number
		// (Confidence criteria to be removed after 3.5.0)
		if (selectedNeighbor == NULL)
		{
			//cout << " Chosen, first suitable neighbor" << endl;
			selectedNeighbor = &(*it);
		}

		else if (it->confidence > selectedNeighbor->confidence)
		{
			//cout << " Chosen, best arrival confidence" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->confidence < selectedNeighbor->confidence)
		{
			//cout << " Not Chosen, bad arrival confidence" << endl;
			continue;
		}
		else if (it->hopCount < selectedNeighbor->hopCount)
		{
			//cout << " Chosen, best hop count" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->hopCount > selectedNeighbor->hopCount)
		{
			//cout << " Not Chosen, bad hop count" << endl;
			continue;
		}
		else if (it->arrivalTime < selectedNeighbor->arrivalTime)
		{
			//cout << " Chosen, best arrival time" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->arrivalTime > selectedNeighbor->arrivalTime)
		{
			//cout << " Not Chosen, bad arrival time" << endl;
			continue;
		}
//		else if(it->meanStartTime < selectedNeighbor->meanStartTime)
//		{
//			selectedNeighbor = &(*it);
//		}
//		else if(it->meanStartTime > selectedNeighbor->meanStartTime)
//		{
//			continue;
//		}
		else if (it->neighborNodeNbr < selectedNeighbor->neighborNodeNbr)
		{
			//cout << " Chosen, best node number" << endl;
			selectedNeighbor = &(*it);
		}
	}

	if (selectedNeighbor != NULL)
	{
		// enqueueToNeighbor() function in ion
		// cout << "  enqueueing to chosen Neighbor" << endl;
		cout << "Best: routeTable[" << bundle->getDestinationEid() << "][" << selectedNeighbor->neighborNodeNbr << "]: nextHop: " << selectedNeighbor->neighborNodeNbr << " (cId:" << selectedNeighbor->contactId << ", resCap:"
				<< contactPlan_->getContactById(selectedNeighbor->contactId)->getResidualVolume() << "Bytes) arrivalConf:" << selectedNeighbor->confidence << " arrivalT:" << selectedNeighbor->arrivalTime << " hopCnt:"
				<< selectedNeighbor->hopCount << " forfT:" << selectedNeighbor->forfeitTime << endl;
		enqueueToNeighbor(bundle, selectedNeighbor);

		// TODO: manageOverbooking() function
		// Only necesary for bundles with priority > bulk.
		return;
	} else
	{
		if ((bundle->getCgrRoute().nextHop != EMPTY_ROUTE))
		{
			changedRoutes++;
		}
	}

	// if the expected confidence level is reached, done
	if (bundle->getDlvConfidence() >= MIN_DTN_DELIVERY_CONFIDENCE)
	{
		cout << "  delivery confidence reached, end cgrForward" << endl;
		// TODO: delete bundle if not forwarded!
		return;
	}

	// if no routes to destination, done
	// TODO: shouldnt send to limbo?
	if (routeList_[bundle->getDestinationEid()].size() == 0)
	{
		// cout << "  delivery confidence not reached but no routes to dst, end cgrForward" << endl;
		//return;
	}

	if (selectedNeighbor != NULL)
	{ // if bundle was enqueued (bundle ->ductXmitElt)
	  // TODO: clone bundle and send it for routing (enqueueToLimbo in ion)
	  // Here we should clone and call cgrForward again (while), Im a genious :)
		cout << "  delivery confidence not reached, clone and repeat forward" << endl;
	}
	else
	{
		cout << "  no chosen neighbor and delivery confidence not reached, enqueueing to limbo" << endl;
		enqueueToLimbo(bundle);
		return;
	}
}

void RoutingCgrModel350_Hops::identifyProximateNodes(BundlePkt * bundle, double simTime, vector<int> excludedNodes, vector<ProximateNode> * proximateNodes)
{
	int terminusNode = bundle->getDestinationEid();

	// If routes are empty for this node, load route list
	if (routeList_[terminusNode].empty() == true)
	{
		// cout << "    routeList to Node:" << terminusNode << " empty, calling loadRouteList" << endl;
		loadRouteList(terminusNode, simTime);
		routeListLastEditTime = simTime;
	}

	//cout << "    routeList to Node:" << terminusNode << " size:" << routeList_[terminusNode].size() << endl;

	for (vector<CgrRoute>::iterator it = routeList_[terminusNode].begin(); it != routeList_[terminusNode].end(); ++it)
	{
		tableEntriesExplored++;

		cout << "*route through node:" << (*it).nextHop << ", arrivalConf:" << (*it).confidence << ", arrivalT:" << (*it).arrivalTime << ", txWin:(" << (*it).fromTime << "-" << (*it).toTime << "), maxCap:" << (*it).maxVolume << "Bytes:"
				<< endl;

		// print route:
		for (vector<Contact *>::iterator ith = (*it).hops.begin(); ith != (*it).hops.end(); ++ith)
			cout << "(+" << (*ith)->getStart() << " +" << (*ith)->getEnd() << " " << (*ith)->getSourceEid() << " " << (*ith)->getDestinationEid() << ")";
		cout << endl;

		if ((*it).toTime <= simTime)
		{
			//cout << " ignoring, route due, recompute route for contact (not implemented yet!)" << endl;
			recomputeRouteForContact();
			// TODO: a new route should be looked and the
			// for loop might need to be restarted if found
			// Now we just ignore the old route (pesimistic)
			continue;
		}

		// If arrival time is after deadline, ignore route
		if ((*it).arrivalTime > bundle->getTtl().dbl())
		{
			//cout << " ignoring, does not satisfies bundle deadline" << endl;
			continue;
		}

		// When a contact happen or is in the contact
		// plan, ion set its confidence to 1.0. Otherwise,
		// it is an opportunistic contact.
		if ((*it).hops[0]->getConfidence() != 1.0)
		{
			//cout << " ignoring, first hop has confidence different than 1" << endl;
			continue;
		}

		// If Im the final destination and the next hop,
		// do not route through myself. Not sure when would
		// this happen.
		if ((*it).nextHop == eid_)
			if (bundle->getDestinationEid() == (*it).nextHop)
			{
				//cout << " ignoring, first hop and destination is this node" << endl;
				continue;
			}

		// If bundle does not fit in route, ignore.
		// With proactive fragmentation, this should not stay.
		if (bundle->getByteLength() > (*it).maxVolume)
		{
			//cout << " ignoring, maxCapacity cannot accomodate bundle" << endl;
			// continue;
		}

		// If bundle does not fit in first contact, ignore.
		if (bundle->getByteLength() > (*it).hops[0]->getResidualVolume())
		{
			continue;
		}

		// If next hop is in excluded nodes, ignore.
		vector<int>::iterator itExl = find(excludedNodes.begin(), excludedNodes.end(), (*it).nextHop);
		if (itExl != excludedNodes.end())
		{
			//cout << " ignoring, next hop is in excludedNodes" << endl;
			continue;
		}

		// If we got to this point, the route might be
		// considered. However, some final tests must be
		// donde before evaluating the node for the proxNodes.
		tryRoute(bundle, &(*it), proximateNodes);
	}
}

void RoutingCgrModel350_Hops::tryRoute(BundlePkt * bundle, CgrRoute * route, vector<ProximateNode> * proximateNodes)
{

	// First, ion test if outduct is blocked,
	// we do not considered blocked outducts here

	// Then, ion test the do-not-fragment flag.
	// if set, and outduct frame size is not enough,
	// return. We do not a frame limit in dtnsim.

	// Thirdly, ion computeArrivalTime() to determine
	// the impact of the outbound queue in the arrival
	// time. We coud do this here also (TODO).
	// We imitate this behaviour by measuring the
	// residual capacity of the first contact.
	if (route->hops[0]->getResidualVolume() < bundle->getByteLength())
	{
		//cout << " residual capacity of first contact in route depleted" << endl;
		return;
	}

	// Last, we go through proximateNodes to add the route
	for (vector<ProximateNode>::iterator it = (*proximateNodes).begin(); it != (*proximateNodes).end(); ++it)
	{
		if ((*it).neighborNodeNbr == route->nextHop)
		{
			// The next-hop is already among proximateNodes.
			// Test if we should update this node metrics.
			// Confidence criteria to be removed in post 3.5.0
			if (route->confidence > (*it).confidence)
			{
				//cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).confidence = route->confidence;
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			else if (route->confidence < (*it).confidence)
			{
				//cout << " route through node " << route->toNodeNbr << " in proxNodes has better arrival confidence" << endl;
				return;
			}
			else if (route->hops.size() < (*it).hopCount)
			{
				//cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			else if (route->hops.size() > (*it).hopCount)
			{
				//cout << " route through node " << route->toNodeNbr << " in proxNodes has better hop count" << endl;
				return;
			}
			else if (route->arrivalTime < (*it).arrivalTime)
			{
				//cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).arrivalTime = route->arrivalTime;
				(*it).hopCount = route->hops.size();
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			else if (route->arrivalTime > (*it).arrivalTime)
			{
				//cout << " route through node " << route->toNodeNbr << " in proxNodes has better arrival time" << endl;
				return;
			}
			else if (route->nextHop < (*it).neighborNodeNbr)
			{
				//cout << " good route, replace node " << route->toNodeNbr << " in proxNodes" << endl;
				(*it).forfeitTime = route->toTime;
				(*it).contactId = route->hops[0]->getId();
				(*it).route = route;
			}
			//cout << " route through node " << route->toNodeNbr << " in proxNodes has same metrics" << endl;
			return;
		}
	}

	// If we got to this point, the node is not in
	// proximateNodes list. So we create and add one.
	// cout << " good route, add node " << route->toNodeNbr << " to proxNodes" << endl;
	ProximateNode node;
	node.neighborNodeNbr = route->nextHop;
	node.confidence = route->confidence;
	node.arrivalTime = route->arrivalTime;
	node.contactId = route->hops[0]->getId();
	node.forfeitTime = route->toTime;
	node.hopCount = route->hops.size();
	node.route = route;
	proximateNodes->push_back(node);
}

void RoutingCgrModel350_Hops::loadRouteList(int terminusNode, double simTime)
{
	// Create rootContact and its corresponding rootWork
	Contact rootContact(0, 0, 0, eid_, eid_, 0, 1.0, 0);
	Work rootWork;
	rootWork.contact = &rootContact;
	rootWork.arrivalTime = simTime;
	rootContact.work = &rootWork;

	// Create route vector in routeList
	vector<CgrRoute> cgrRoute;
	routeList_[terminusNode] = cgrRoute;

	// Create and initialize working area in each contact
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
	{
		(*it).work = new Work;
		((Work *) (*it).work)->contact = &(*it);
		((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
		((Work *) (*it).work)->capacity = 0;
		((Work *) (*it).work)->predecessor = 0;
		((Work *) (*it).work)->visited = false;
		((Work *) (*it).work)->suppressed = false;
	}

	Contact * anchorContact = NULL;
	while (1)
	{
		// Find the next best route
		CgrRoute route;
		route.nextHop = 0;
		findNextBestRoute(&rootContact, terminusNode, &route);

		// If toNodeNbr still 0, no routes were found
		// End search
		if (route.nextHop == 0)
		{
			// cout << "      no more routes found" << endl;
			break;
		}

		// If anchored search on going and firstContact
		// is not anchor, end the anchor and do not record
		// this route.
		Contact * firstContact = route.hops[0];
		if (anchorContact != NULL)
			if (firstContact != anchorContact)
			{
				// cout << "        ending anchored search in contactId: " << anchorContact->getId() << " (src:" << anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid() << ", " << anchorContact->getStart() << "s to " << anchorContact->getEnd() << "s)" << endl;
				// This is endAnchoredSearch() function in ion: it clears the working area
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
				{
					((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
					((Work *) (*it).work)->predecessor = NULL;
					((Work *) (*it).work)->visited = false;
					// Unsupress all non-local contacts
					// (local contacts must keep their value).
					if ((*it).getSourceEid() != eid_)
						((Work *) (*it).work)->suppressed = false;
				}
				// End endAnchoredSearch() function
				((Work *) anchorContact->work)->suppressed = 1;
				anchorContact = NULL;
				continue;
			}

		// Record route
		cout << "NODE " << eid_ << ", *New route found through node:" << route.nextHop << ", arrivalConf:" << route.confidence << ", arrivalT:" << route.arrivalTime << ", txWin:(" << route.fromTime << "-" << route.toTime << "), maxCap:"
				<< route.maxVolume << "Bytes:" << endl;
		routeList_[terminusNode].push_back(route);

		// Find limiting contact for next iteration
		// (earliest ending contact in path, generally the first)
		Contact * limitContact = NULL;
		if (route.toTime == firstContact->getEnd())
		{
			limitContact = firstContact;
		}
		else
		{
			// Start new anchor search. Anchoring only
			// happens in the first hop.. not good!
			anchorContact = firstContact;
			// cout << "        starting anchored search in contactId: " << anchorContact->getId() << " (src:" << anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid() << ", " << anchorContact->getStart() << "s-" << anchorContact->getEnd() << "s)" << endl;
			// find the limiting contact in route
			for (vector<Contact *>::iterator it = route.hops.begin(); it != route.hops.end(); ++it)
				if ((*it)->getEnd() == route.toTime)
				{
					limitContact = (*it);
					break;
				}
		}

		// Supress limiting contact in next search
		// cout << "        supressing limiting contactId: " << limitContact->getId() << " (src:" << limitContact->getSourceEid() << "-dst:" << limitContact->getDestinationEid() << ", " << limitContact->getStart() << "s-" << limitContact->getEnd() << "s)" << endl;
		((Work *) limitContact->work)->suppressed = true;

		// Clear working area
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		{
			((Work *) (*it).work)->arrivalTime = numeric_limits<double>::max();
			((Work *) (*it).work)->predecessor = 0;
			((Work *) (*it).work)->visited = false;
		}
	}

	// Free memory allocated for work
	for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		delete ((Work *) (*it).work);

}

void RoutingCgrModel350_Hops::findNextBestRoute(Contact * rootContact, int terminusNode, CgrRoute * route)
{
	// increment counter
	dijkstraCalls++;

	// If toNodeNbr remains equal to 0, it means no
	// route was found by this function. In ion this
	// is signaled by a null psm address.

	// This is the computeDistanceToTerminus() in ion:
	Contact * currentContact = rootContact;
	Contact * finalContact = NULL;
	double earliestFinalArrivalTime = numeric_limits<double>::max();
	float highestConfidence = 0.0;

	// cout << "        surfing contact-graph:";

	while (1)
	{
		// increment counter
		dijkstraLoops++;

		// Go thorugh all next hop neighbors in the
		// contact plan (all contacts which source
		// node is the currentWork destination node)

		// cout << currentContact->getDestinationEid() << ",";
		vector<Contact> currentNeighbors = contactPlan_->getContactsBySrc(currentContact->getDestinationEid());
		for (vector<Contact>::iterator it = currentNeighbors.begin(); it != currentNeighbors.end(); ++it)
		{
			// First, check if contact needs to be considered
			// in ion an if (contact->fromNode > arg.fromNode)
			// is necesary due to the red-black tree stuff. Not here :)

			// This contact is finished, ignore it.
			if ((*it).getEnd() <= ((Work *) (currentContact->work))->arrivalTime)
				continue;

			// This contact is suppressed/visited, ignore it.
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
				continue;

			// Get owlt (one way light time). If none found, ignore contact
			double owlt = contactPlan_->getRangeBySrcDst((*it).getSourceEid(), (*it).getDestinationEid());
			if (owlt == -1)
			{
				cout << "warning, range not available for nodes " << (*it).getSourceEid() << "-" << (*it).getDestinationEid() << ", assuming range=0" << endl;
				owlt = 0;
			}
			//double owltMargin = ((MAX_SPEED_MPH / 3600) * owlt) / 186282;
			//owlt += owltMargin;

			// Calculate capacity
			// TODO: This capacity calculation should be then
			// updated based on the start of the effective
			// usage of the contact
			if (((Work *) (*it).work)->capacity == 0)
				((Work *) (*it).work)->capacity = (*it).getDataRate() * (*it).getDuration();

			// Calculate the cost for this contact (Arrival Time)
			double arrivalTime;
			if ((*it).getStart() < ((Work *) (currentContact->work))->arrivalTime)
				arrivalTime = ((Work *) (currentContact->work))->arrivalTime;
			else
				arrivalTime = (*it).getStart();
			arrivalTime += owlt;

			// Update the cost of this contact
			if (arrivalTime < ((Work *) (*it).work)->arrivalTime)
			{
				((Work *) (*it).work)->arrivalTime = arrivalTime;
				((Work *) (*it).work)->predecessor = currentContact;

				// If this contact reaches the terminus node
				// consider it as final contact
				if ((*it).getDestinationEid() == terminusNode)
				{

					// Confidence criteria to be removed in post 3.5.0
					if ((*it).getConfidence() > highestConfidence || (((*it).getConfidence() == highestConfidence) && ((Work *) (*it).work)->arrivalTime < earliestFinalArrivalTime))
					{
						highestConfidence = (*it).getConfidence();
						earliestFinalArrivalTime = ((Work *) (*it).work)->arrivalTime;
						// Warning: we need to point finalContact to
						// the real contact in contactPlan. This iteration
						// goes over a copy of the original contact plan
						// returned by getContactsBySrc().
						finalContact = contactPlan_->getContactById((*it).getId());
					}
				}
			}
		} // end for (currentNeighbors)

		((Work *) (currentContact->work))->visited = true;

		// Select next (best) contact to move to in next iteration
		Contact * nextContact = NULL;
		double earliestArrivalTime = numeric_limits<double>::max();
		for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin(); it != contactPlan_->getContacts()->end(); ++it)
		{
			// Do not evaluate suppressed or visited contacts
			if (((Work *) (*it).work)->suppressed || ((Work *) (*it).work)->visited)
				continue;

			// If the arrival time is worst than the best found so far, ignore
			if (((Work *) (*it).work)->arrivalTime > earliestFinalArrivalTime)
				continue;

			// Then this might be the best candidate contact
			if (((Work *) (*it).work)->arrivalTime < earliestArrivalTime)
			{
				nextContact = &(*it);
				earliestArrivalTime = ((Work *) (*it).work)->arrivalTime;
			}
		}
		if (nextContact == NULL)
			break; // No next contact found, exit search

		// Update next contact and go with next itartion
		currentContact = nextContact;
	} // end while (1)

	// cout << endl;

	// If we got a final contact to destination
	// then it is the best route and we need to
	// translate the info from the work area to
	// the CgrRoute route pointer.
	if (finalContact != NULL)
	{
		route->arrivalTime = earliestFinalArrivalTime;
		route->confidence = 1.0;

		double earliestEndTime = numeric_limits<double>::max();
		double maxCapacity = numeric_limits<double>::max();

		// Go through all contacts in the path
		for (Contact * contact = finalContact; contact != rootContact; contact = ((Work *) (*contact).work)->predecessor)
		{
			// Get earliest end time
			if (contact->getEnd() < earliestEndTime)
				earliestEndTime = contact->getEnd();

			// Get the minimal capacity
			if (((Work *) (*contact).work)->capacity < maxCapacity)
				maxCapacity = ((Work *) (*contact).work)->capacity;

			// Update confidence
			route->confidence *= contact->getConfidence();

			// Store hop:
			//route->hops.push_back(contact);
			route->hops.insert(route->hops.begin(), contact);
		}

		route->nextHop = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->toTime = earliestEndTime;
		route->maxVolume = maxCapacity;
	}
}

void RoutingCgrModel350_Hops::recomputeRouteForContact()
{
	//cout << "***RecomputeRouteForContact not implemented yet!, ignoring route***" << endl;
}

void RoutingCgrModel350_Hops::enqueueToNeighbor(BundlePkt * bundle, ProximateNode * selectedNeighbor)
{

	if (bundle->getXmitCopiesCount() == MAX_XMIT_COPIES)
		return;
	bundle->setXmitCopiesCount(bundle->getXmitCopiesCount() + 1);

	float newDlvConfidence = 1.0 - (1.0 - bundle->getDlvConfidence()) * (1.0 - selectedNeighbor->confidence);
	bundle->setDlvConfidence(newDlvConfidence);

	// In ion, bpEnqueue goes directly
	// to SDR duct (bpEnqueue() in libbpP.c)
	// For us this goes to the neighbor queue.
	bpEnqueue(bundle, selectedNeighbor);
}

void RoutingCgrModel350_Hops::enqueueToLimbo(BundlePkt * bundle)
{
	ProximateNode limboNode;
	limboNode.contactId = 0;
	limboNode.neighborNodeNbr = 0;

	// In ion, enqueueToLimbo goes directly
	// to SDR limbo (enqueueToLimbo() in libbpP.c)
	// For us this goes to queue 0.
	bpEnqueue(bundle, &limboNode);
}

void RoutingCgrModel350_Hops::bpEnqueue(BundlePkt * bundle, ProximateNode * selectedNeighbor)
{
	bundle->setNextHopEid(selectedNeighbor->neighborNodeNbr);

	if (selectedNeighbor->contactId != 0)
	{
		// if the bundle does not has a route in the header
		// set route in header for statistical purposes
		if ((bundle->getCgrRoute().nextHop == EMPTY_ROUTE))
		{
			bundle->setCgrRoute(*(selectedNeighbor->route));
		}
		// else, check if the selected neighbor belongs to the previously assigned route
		else
		{
			bool changedRoute = false;

			vector<Contact *> hops = bundle->getCgrRoute().hops;

			for (size_t i = 0; i < hops.size(); i++)
			{
				Contact *contact = hops.at(i);
				if (contact->getSourceEid() == this->eid_)
				{
					if (contact->getId() != selectedNeighbor->contactId)
					{
						changedRoute = true;
					}
					break;
				}
			}

			if (changedRoute)
			{
				bundle->setCgrRoute(*(selectedNeighbor->route));
				changedRoutes++;
			}
		}
	}

	sdr_->enqueueBundleToContact(bundle, selectedNeighbor->contactId);

	if (selectedNeighbor->contactId != 0)
	{
		// Decrease first contact capacity:
		selectedNeighbor->route->hops[0]->setResidualVolume(selectedNeighbor->route->hops[0]->getResidualVolume() - bundle->getByteLength());

		cout << "RVol: routeTable[" << bundle->getDestinationEid() << "][" << selectedNeighbor->neighborNodeNbr << "]: new resCap: (cId:" << selectedNeighbor->contactId << ", resCap:"
				<< contactPlan_->getContactById(selectedNeighbor->contactId)->getResidualVolume() << "Bytes)" << endl;

		// Decrease route capacity:
		// It seems this does not happen in ION. In fact, the
		// queiung process considers the local outbound capacity, which
		// is analogous to consider the first contact capacity. However,
		// there is chance to also consider remote contact capacity as
		// in PA-CGR. Furthermore, the combined effect of routeList
		// and PA-CGR need to be investigated because an update from
		// one route might impact other routes that uses the same contacts.
		selectedNeighbor->route->maxVolume -= bundle->getByteLength();

		EV << "Node " << eid_ << ": bundle to node " << bundle->getDestinationEid() << " enqueued in queueId: " << selectedNeighbor->contactId << " (next hop: " << selectedNeighbor->neighborNodeNbr << ")" << endl;
	}
	else
	{
		EV << "Node " << eid_ << ": bundle to node " << bundle->getDestinationEid() << " enqueued to limbo!" << endl;
	}
}

CgrRoute* RoutingCgrModel350_Hops::getCgrBestRoute(BundlePkt * bundle, double simTime)
{
	// modified from method cgrForward()

	CgrRoute *bestRoute = NULL;

	// If contact plan was changed, discard route list
	if (contactPlan_->getLastEditTime() > routeListLastEditTime)
		routeList_.clear();

	vector<ProximateNode> proximateNodes;
	vector<int> excludedNodes;

	// Insert sender node to excludedNodes to avoid loops
	if (bundle->getReturnToSender() == false)
		excludedNodes.push_back(bundle->getSenderEid());

	// Insert other nodes into excludedNodes (embargoed nodes)
	// Not happening in the DtnSim

	// Populate proximateNodes
	// cout << "  calling identifyProximateNodes: " << endl;
	identifyProximateNodes(bundle, simTime, excludedNodes, &proximateNodes);

	// TODO: send critical bundle to all proximateNodes
	if (bundle->getCritical())
	{
		cout << "***Critical bundle forwarding not implemented yet!***" << endl;
		exit(1);
	}

	//cout << "  proximateNodesSize: " << proximateNodes.size() << ":" << endl;

	ProximateNode * selectedNeighbor = NULL;
	for (vector<ProximateNode>::iterator it = proximateNodes.begin(); it != proximateNodes.end(); ++it)
	{
		cout << "routeTable[" << bundle->getDestinationEid() << "][" << (*it).neighborNodeNbr << "]: nextHop: " << (*it).neighborNodeNbr << " (cId:" << (*it).contactId << ", resCap:"
				<< contactPlan_->getContactById((*it).contactId)->getResidualVolume() << "Bytes) arrivalConf:" << (*it).confidence << " arrivalT:" << (*it).arrivalTime << " hopCnt:" << (*it).hopCount << " forfT:" << (*it).forfeitTime
				<< endl;
		//cout << "routeTable[" << terminusNode << "][" << (*it).neighborNodeNbr << "]: nextHop: " << (*it).neighborNodeNbr << ", frm " << route.fromTime << " to " << (*it).arrivalTime << ", arrival time: " << route.arrivalTime << ", volume: " << route.residualVolume << "/" << route.maxVolume << "Bytes" << endl;

		// If the confidence improvement is less than the minimal, continue
		if (bundle->getDlvConfidence() > 0.0 && bundle->getDlvConfidence() < 1.0)
		{
			float newDlvConfidence = 1.0 - (1.0 - bundle->getDlvConfidence()) * (1.0 - it->confidence);
			float confidenceImprovement = (newDlvConfidence / bundle->getDlvConfidence()) - 1.0;
			if (confidenceImprovement < MIN_CONFIDENCE_IMPROVEMENT)
			{
				//cout << " Not chosen, not enough confidence improvement" << endl;
				continue;
			}
		}

		// Select Neighbor by arrival confidence, arrival time, hop count, and node number
		// (Confidence criteria to be removed after 3.5.0)
		if (selectedNeighbor == NULL)
		{
			//cout << " Chosen, first suitable neighbor" << endl;
			selectedNeighbor = &(*it);
		}

		else if (it->confidence > selectedNeighbor->confidence)
		{
			//cout << " Chosen, best arrival confidence" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->confidence < selectedNeighbor->confidence)
		{
			//cout << " Not Chosen, bad arrival confidence" << endl;
			continue;
		}
		else if (it->arrivalTime < selectedNeighbor->arrivalTime)
		{
			//cout << " Chosen, best arrival time" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->arrivalTime > selectedNeighbor->arrivalTime)
		{
			//cout << " Not Chosen, bad arrival time" << endl;
			continue;
		}
		else if (it->hopCount < selectedNeighbor->hopCount)
		{
			//cout << " Chosen, best hop count" << endl;
			selectedNeighbor = &(*it);
		}
		else if (it->hopCount > selectedNeighbor->hopCount)
		{
			//cout << " Not Chosen, bad hop count" << endl;
			continue;
		}
		else if (it->neighborNodeNbr < selectedNeighbor->neighborNodeNbr)
		{
			//cout << " Chosen, best node number" << endl;
			selectedNeighbor = &(*it);
		}
	}

	if (selectedNeighbor != NULL)
	{
		bestRoute = selectedNeighbor->route;
	}

	return bestRoute;
}

vector<CgrRoute> RoutingCgrModel350_Hops::getCgrRoutes(BundlePkt * bundle, double simTime)
{
	if (!printDebug_) // disable cout if degug disabled
		cout.setstate(std::ios_base::failbit);

	vector<CgrRoute> routes;

	int terminusNode = bundle->getDestinationEid();
	if (routeList_[terminusNode].empty() == true)
	{
		loadRouteList(terminusNode, simTime);
		routeListLastEditTime = simTime;
	}

	map<int, vector<CgrRoute> >::iterator it = routeList_.find(terminusNode);
	if (it != routeList_.end())
	{
		routes = it->second;
	}

	if (!printDebug_)
		cout.clear();

	return routes;
}

//////////////////////
// Stats recollection
//////////////////////

int RoutingCgrModel350_Hops::getDijkstraCalls()
{
	return dijkstraCalls;
}

int RoutingCgrModel350_Hops::getDijkstraLoops()
{
	return dijkstraLoops;
}

int RoutingCgrModel350_Hops::getRouteTableEntriesExplored()
{
	return tableEntriesExplored;
}

int RoutingCgrModel350_Hops::getChangedRoutes()
{
	return changedRoutes;
}

