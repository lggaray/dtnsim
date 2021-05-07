#include <src/node/dtn/routing/RoutingCgrModelRev17.h>

// This function initializes the routing class:
// To this end, local eid_, the total number of nodes nodeNum_,
// a pointer to local storage sdr_, and a contact plan are set.
RoutingCgrModelRev17::RoutingCgrModelRev17(int eid, int nodeNum, SdrModel * sdr, ContactPlan * localContactPlan,
		ContactPlan * globalContactPlan, string routingType, bool printDebug)
	: RoutingDeterministic(eid, sdr, NULL)
{
	// Initialize basic and default variables
	nodeNum_ = nodeNum;				// number of neighbors
	printDebug_ = printDebug;		// direct debug to cout
	routingType_ = routingType;		// routing type string

	// Check routingType string is correct
	this->checkRoutingTypeString();

	// Set global or local contact plan
	if (routingType_.find("contactPlan:local") != std::string::npos)
		contactPlan_ = localContactPlan;
	else if (routingType_.find("contactPlan:global") != std::string::npos)
		contactPlan_ = globalContactPlan;

	// Initialize routeTable_: it will have nodeNum_ entries, one entry
	// per each possible neighbor node. Each entry will host a routeList
	routeTable_.resize(nodeNum_);
}

RoutingCgrModelRev17::~RoutingCgrModelRev17() {

}

// This function is called every time a new bundle (local or
// in transit) need to be routed.The outcome of the function
// is to enqueue the bundle in the SDR memory which is organized
// by a set of queues addressed by queueIds. In current DtnSim
// version Ids corresponds with the contact Id where the bundle
// is expected to be forwarded. This mimic ION behaviour. Other
// implementations do enqueue bundles on a per neighbour-node basis.
void RoutingCgrModelRev17::routeAndQueueBundle(BundlePkt * bundle, double simTime) {

	// Disable cout if degug disabled
	if (printDebug_ == false)
		cout.setstate(std::ios_base::failbit);

	// Set local time
	simTime_ = simTime;

	// Reset counters (metrics)
	dijkstraCalls = 0;
	dijkstraLoops = 0;
	tableEntriesCreated = 0;
	tableEntriesExplored = 0;
	bundlesInLimbo = 0;

	cout << "TIME: " << simTime_ << "s, NODE: " << eid_ << ", routing bundle : " << bundle->getSourceEid() << "-"
			<< bundle->getDestinationEid() << " (" << bundle->getByteLength() << "Bytes)" << endl;

	// If no extensionBlock, run cgr each time a bundle is dispatched
	if (routingType_.find("extensionBlock:off") != std::string::npos)
		this->cgrForward(bundle);

	// If extensionBlock, check header
	if (routingType_.find("extensionBlock:on") != std::string::npos) {
		if (bundle->getCgrRoute().nextHop == EMPTY_ROUTE) {
			// Empty extension block: Calculate, encode a new path and enqueue
			this->cgrForward(bundle);
		} else {
			// Non-empty EB: Use EB Route to route bundle
			CgrRoute ebRoute = bundle->getCgrRoute();

			// Print route and hops
			cout << "extensionBlockRoute: [" << ebRoute.terminusNode << "][" << ebRoute.nextHop << "]: nextHop: "
					<< ebRoute.nextHop << ", frm " << ebRoute.fromTime << " to " << ebRoute.toTime << ", arrival time: "
					<< ebRoute.arrivalTime << ", volume: " << ebRoute.residualVolume << "/" << ebRoute.maxVolume
					<< "Bytes" << endl << "hops: ";
			for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop)
				cout << "(+" << (*hop)->getStart() << " +" << (*hop)->getEnd() << " " << (*hop)->getSourceEid() << " "
						<< (*hop)->getDestinationEid() << " " << (*hop)->getResidualVolume() << "/"
						<< (*hop)->getVolume() << "Bytes-local:"
						<< contactPlan_->getContactById((*hop)->getId())->getResidualVolume() << "/"
						<< contactPlan_->getContactById((*hop)->getId())->getVolume() << "Bytes)";
			cout << endl;

			// Check if encoded path is valid, update local contacts and route hops
			bool ebRouteIsValid = true;

			// Reason 1) If this bundle first contact is not a contact to this node, something
			// weird happened (might be a bundle re-route). Declare ebRoute invalid.
			if (ebRoute.hops.at(0)->getDestinationEid() != eid_)
				ebRouteIsValid = false;

			// Reason 2) If the remaining volume of local contact plan cannot
			// accommodate the encoded path, declare ebRout invalid.
			vector<Contact *> newHops;
			for (vector<Contact *>::iterator hop = ebRoute.hops.begin(); hop != ebRoute.hops.end(); ++hop) {

				// TODO: Fuse the ebRoute information with local contact graph
				// This should provide an improved behavior in between the
				// global and local contact plan extension block configuration
				// But beware that this hops points to a remote contact graph.

				if ((*hop)->getDestinationEid() == eid_)
					// This is the contact that made this bundle arrive here,
					// do not check nothing and discard it from the newHops path
					continue;

				// Check volumes only when using a local contact plan perspective.
				// When using global, the sender node will have already booked the
				// capacity so we cannot and there is no need to verify this decision.
				if (routingType_.find("contactPlan:local") != std::string::npos) {

					if (routingType_.find("volumeAware:1stContact") != std::string::npos)
						// Only check first contact volume
						if ((*hop)->getSourceEid() == eid_)
							if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume()
									< bundle->getByteLength()) {
								// Not enough residual capacity from local view of the path
								ebRouteIsValid = false;
								break;
							}

					if (routingType_.find("volumeAware:allContacts") != std::string::npos)
						// Check all contacts volume
						if (contactPlan_->getContactById((*hop)->getId())->getResidualVolume()
								< bundle->getByteLength()) {
							// Not enough residual capacity from local view of the path
							ebRouteIsValid = false;
							break;
						}
				}

				// store in newHops the local contacts
				newHops.push_back(contactPlan_->getContactById((*hop)->getId()));
			}

			if (ebRouteIsValid) {
				// Update necessary route parameters (from/to time and max/residual volume not necessary)
				ebRoute.hops = newHops;
				ebRoute.nextHop = ebRoute.hops[0]->getDestinationEid();

				// Enqueue using this route
				cout << "Using EB Route in header. Next hop: " << ebRoute.nextHop << endl;
				this->cgrEnqueue(bundle, &ebRoute);
			} else {
				// Discard old extension block, calculate and encode a new path and enqueue
				cout << "EB Route in header not valid, generating a new one" << endl;
				this->cgrForward(bundle);
			}
		}
	}

	// Re-enable cout if debug disabled
	if (printDebug_ == false)
		cout.clear();
}

/////////////////////////////////////////////////
// Functions based Ion architecture
/////////////////////////////////////////////////

// This function tries to find the best path for the current bundle.
// Initially it checks if the route table is up-to-date and update it
// if it is out of date (by running a new Dijkstra search for the
// corresponding neighbor).
void RoutingCgrModelRev17::cgrForward(BundlePkt * bundle) {
	// If contact plan was changed, empty route list
	if (contactPlan_->getLastEditTime() > routeTableLastEditTime)
		this->clearRouteTable();
	routeTableLastEditTime = simTime_;

	int terminusNode = bundle->getDestinationEid();

	// When using global contact plan, other nodes nodes might have
	// updated contacts volumes that need to be refreshed in local
	// route table.
	if (routingType_.find("contactPlan:global") != std::string::npos) {

		if (!routeTable_.at(terminusNode).empty())
			for (unsigned int r = 0; r < routeTable_.at(terminusNode).size(); r++)
				for (vector<Contact *>::iterator hop1 = routeTable_.at(terminusNode).at(r).hops.begin();
						hop1 != routeTable_.at(terminusNode).at(r).hops.end(); ++hop1)
					if (routeTable_.at(terminusNode).at(r).residualVolume > (*hop1)->getResidualVolume()) {
						routeTable_.at(terminusNode).at(r).residualVolume = (*hop1)->getResidualVolume();
						cout << "*Rvol: routeTable[" << terminusNode << "][" << r << "]: updated to "
								<< (*hop1)->getResidualVolume() << "Bytes (all contacts)" << endl;
					}

	}

	// Check route list for terminus node and recalculate if necesary depending on
	// the routeListType configured in the routingType string
	if (routingType_.find("routeListType:allPaths-yen") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-yen: All paths are calculated at once
		// when the contact plan changes. The route list
		// is completed using yen algorithm.
		//////////////////////////////////////////////////

		cout << "dtnsim error: routeListType:allPaths-yen not implemented yet" << endl;
		exit(1);

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;

			// Determine the shortest path and add it to routeList
			CgrRoute route;
			this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

			// Work on-going here...

		}
	}
	if (routingType_.find("routeListType:allPaths-initial+anchor") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-initial+anchor: All paths are calculated
		// at once when the contact plan changes. The route
		// list is completed using remove initial + anchor
		// algorithm currently used in ION 3.6.0.
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;
			Contact * anchorContact = NULL;

			while (1) {
				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

				// If no more routes were found, stop search loop
				if (route.nextHop == NO_ROUTE_FOUND)
					break;

				Contact * firstContact = route.hops.at(0);

				// If anchor search on-going
				if (anchorContact != NULL)
					// And the first contact is no longer anchor
					if (firstContact != anchorContact) {
						// End anchor search: [endAnchoredSearch() function in ion]

						cout << "-anchored search ends in contactId: " << anchorContact->getId() << " (src:"
								<< anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid()
								<< ", " << anchorContact->getStart() << "s to " << anchorContact->getEnd() << "s)"
								<< endl;

						// Unsupress all remote contacts (suppressed local contacts dont change)
						for (vector<int>::iterator it = suppressedContactIds.begin(); it != suppressedContactIds.end();)
							if ((contactPlan_->getContactById((*it)))->getSourceEid() != eid_)
								it = suppressedContactIds.erase(it);
							else
								++it;

						// Suppress anchorContact from further searches.
						suppressedContactIds.push_back(anchorContact->getId());
						anchorContact = NULL;

						// Ignore the latest route and continue with next iteration
						continue;
					}

				// Add new valid route to route table
				routeTable_.at(terminusNode).push_back(route);
				cout << "-new route found through node:" << route.nextHop << ", arrivalConf:" << route.confidence
						<< ", arrivalT:" << route.arrivalTime << ", txWin:(" << route.fromTime << "-" << route.toTime
						<< "), maxCap:" << route.maxVolume << "Bytes:" << endl;

				// Find limiting contact
				Contact * limitContact = NULL;
				if (route.toTime == firstContact->getEnd())
					// Generally it is the first
					limitContact = firstContact;
				else {
					// If not, start new anchor search on firstContact
					anchorContact = firstContact;

					cout << "-anchored search starts in contactId: " << anchorContact->getId() << " (src:"
							<< anchorContact->getSourceEid() << "-dst:" << anchorContact->getDestinationEid() << ", "
							<< anchorContact->getStart() << "s-" << anchorContact->getEnd() << "s)" << endl;

					// Find the limiting contact in route
					for (vector<Contact *>::iterator it = route.hops.begin(); it != route.hops.end(); ++it)
						if ((*it)->getEnd() == route.toTime) {
							limitContact = (*it);
							break;
						}
				}

				// Suppress limiting contact
				suppressedContactIds.push_back(limitContact->getId());

				cout << "-suppressing limiting contactId: " << limitContact->getId() << " (src:"
						<< limitContact->getSourceEid() << "-dst:" << limitContact->getDestinationEid() << ", "
						<< limitContact->getStart() << "s-" << limitContact->getEnd() << "s)" << endl;

				tableEntriesCreated++;
			}
		}
	}
	if (routingType_.find("routeListType:allPaths-firstEnding") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-firstEnding: All paths are calculated at once
		// when the contact plan changes. The route list
		// is completed removing the first ending contact.
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			vector<int> suppressedContactIds;

			while (1) {
				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

				// If no more routes were found, stop search loop
				if (route.nextHop == NO_ROUTE_FOUND)
					break;

				// Add new valid route to route table
				routeTable_.at(terminusNode).push_back(route);

				// Suppress the first ending contact of the last route found
				double earliestEndingTime = numeric_limits<double>::max();
				int earliestEndingContactId;
				vector<Contact *>::iterator hop;
				for (hop = route.hops.begin(); hop != route.hops.end(); ++hop)
					if ((*hop)->getEnd() < earliestEndingTime) {
						earliestEndingTime = (*hop)->getEnd();
						earliestEndingContactId = (*hop)->getId();
					}
				suppressedContactIds.push_back(earliestEndingContactId);

				tableEntriesCreated++;
			}
		}
	}
	if (routingType_.find("routeListType:allPaths-firstDepleted") != std::string::npos) {
		//////////////////////////////////////////////////
		// allPaths-firstDepleted: All paths are calculated at once
		// when the contact plan changes. The route list
		// is completed removing the first depleted contact.
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// populate it with all paths to the destination. This
		// should only happen once per contact plan update.
		if (routeTable_.at(terminusNode).empty()) {

			// Temporarily store the residual capacity of each
			// contact so we can use in the route calculation
			// stage to determine how this capacity would get
			// occupied as traffic is sent to this target node.
			vector<double> contactVolume(contactPlan_->getContacts()->size());
			vector<double>::iterator it1 = contactVolume.begin();
			vector<Contact>::iterator it2 = contactPlan_->getContacts()->begin();
			for (; it1 != contactVolume.end(); ++it1, ++it2)
				(*it1) = (*it2).getResidualVolume();

			vector<int> suppressedContactIds;

			while (1) {
				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);

				// If no more routes were found, stop search loop
				if (route.nextHop == NO_ROUTE_FOUND)
					break;

				// Add new valid route to route table
				routeTable_.at(terminusNode).push_back(route);

				// Consume route residual volume and suppress the contact
				// with least residual volume on the route
				vector<Contact *>::iterator hop;
				for (hop = route.hops.begin(); hop != route.hops.end(); ++hop) {

					// Consume route residual volume
					(*hop)->setResidualVolume((*hop)->getResidualVolume() - route.residualVolume);

					// If this is the limiting contact, suppress
					// it from further route searches (other contacts
					// in the path will remain with a reduced residual
					// volume.
					if ((*hop)->getResidualVolume() == 0) {
						suppressedContactIds.push_back((*hop)->getId());
					}
				}
				tableEntriesCreated++;
			}

			// Restore original residual capacities in the contact plan
			it1 = contactVolume.begin();
			it2 = contactPlan_->getContacts()->begin();
			for (; it1 != contactVolume.end(); ++it1, ++it2)
				(*it2).setResidualVolume(*it1);
		}

	}
	if (routingType_.find("routeListType:oneBestPath") != std::string::npos) {
		//////////////////////////////////////////////////
		// oneBestPath: Only a best path is
		// calculated for a given destination. The entry
		// is updated by the ending or depletion of a contact in the path
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// create a single entry place to hold the route
		if (routeTable_.at(terminusNode).empty()) {
			routeTable_.at(terminusNode).resize(2);
			CgrRoute route;
			route.nextHop = EMPTY_ROUTE;
			route.arrivalTime = numeric_limits<double>::max(); // never chosen as best route
			routeTable_.at(terminusNode).at(0) = route;
			routeTable_.at(terminusNode).at(1) = route;
		}

		// Explore list and recalculate if necessary
		bool needRecalculation = false;

		// Empty route condition
		if (routeTable_.at(terminusNode).at(0).nextHop == EMPTY_ROUTE) {
			cout << "needRecalculation = true 1" << endl;
			needRecalculation = true;
		}

		// Due route condition
		// todo
		if (routeTable_.at(terminusNode).at(0).toTime < simTime_) {
			cout << "needRecalculation = true 2" << endl;
			needRecalculation = true;
		}

		// Depleted route condition
		if (routeTable_.at(terminusNode).at(0).residualVolume < bundle->getByteLength()) {
			// Make sure that the capacity-limiting contact is marked as depleted
			vector<Contact *>::iterator hop;
			for (hop = routeTable_.at(terminusNode).at(0).hops.begin();
					hop != routeTable_.at(terminusNode).at(0).hops.end(); ++hop)
				if (routeTable_.at(terminusNode).at(0).residualVolume == (*hop)->getResidualVolume())
					(*hop)->setResidualVolume(0);

			cout << "needReclculation = true 3" << endl;
			needRecalculation = true;
		}

		if (needRecalculation) {
			vector<int> suppressedContactIds; // no suppressed contacts here
			CgrRoute route;
			this->findNextBestRoute(suppressedContactIds, terminusNode, &route);
			routeTable_.at(terminusNode).at(0) = route;

			// Also create a secondary route thorugh a different entry node
			// in order to have an alternative when return to sender is forbidden
			// through this entry node. set not found if not primary route found.

			if (route.nextHop != NO_ROUTE_FOUND) {
				tableEntriesCreated++;

				// Suppress all contacts which connect this node with the entry node of
				// the route found. All other neighbors should be considered
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
						it != contactPlan_->getContacts()->end(); ++it)
					if ((*it).getSourceEid() == eid_ && (*it).getDestinationEid() == route.nextHop)
						suppressedContactIds.push_back((*it).getId());
				CgrRoute route2;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route2);
				routeTable_.at(terminusNode).at(1) = route2;

				if (route2.nextHop != NO_ROUTE_FOUND)
					tableEntriesCreated++;

			} else {
				routeTable_.at(terminusNode).at(1).nextHop = NO_ROUTE_FOUND;
			}
		} else {
			// Principal route is still valid. Now check if
			// secondary (backup) route needs recalculation
			bool needRecalculation = false;

			// Empty route condition
			if (routeTable_.at(terminusNode).at(1).nextHop == EMPTY_ROUTE)
				needRecalculation = true;

			// Due route condition
			// todo
			if (routeTable_.at(terminusNode).at(1).toTime <= simTime_)
				needRecalculation = true;

			// Depleted route condition
			if (routeTable_.at(terminusNode).at(1).residualVolume < bundle->getByteLength()) {
				// Make sure that the capacity-limiting contact is marked as depleted
				vector<Contact *>::iterator hop;
				for (hop = routeTable_.at(terminusNode).at(1).hops.begin();
						hop != routeTable_.at(terminusNode).at(1).hops.end(); ++hop)
					if (routeTable_.at(terminusNode).at(1).residualVolume == (*hop)->getResidualVolume())
						(*hop)->setResidualVolume(0);

				needRecalculation = true;
			}

			if (needRecalculation) {
				// Suppress all contacts which connect this node with the entry node of
				// the route found. All other neighbors should be considered
				vector<int> suppressedContactIds;
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
						it != contactPlan_->getContacts()->end(); ++it)
					if ((*it).getSourceEid() == eid_
							&& (*it).getDestinationEid() == routeTable_.at(terminusNode).at(0).nextHop)
						suppressedContactIds.push_back((*it).getId());
				CgrRoute route2;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route2);
				routeTable_.at(terminusNode).at(1) = route2;

				if (route2.nextHop != NO_ROUTE_FOUND)
					tableEntriesCreated++;
			}
		}
	}
	if (routingType_.find("routeListType:perNeighborBestPath") != std::string::npos) {
		//////////////////////////////////////////////////
		// perNeighborBestPath: A best path per
		// neighbor is calculated for a given destination. The entry
		// is updated by the ending or depletion of a contact in the paths
		//////////////////////////////////////////////////

		// If this is the route list to this terminus is empty,
		// create list with nodeNum_ EMPTY_ROUTE entries
		if (routeTable_.at(terminusNode).empty()) {
			routeTable_.at(terminusNode).resize(nodeNum_);
			for (int r = 0; r < nodeNum_; r++) {
				CgrRoute route;
				route.nextHop = EMPTY_ROUTE;
				route.arrivalTime = numeric_limits<double>::max(); // never chosen as best route
				routeTable_.at(terminusNode).at(r) = route;
			}
		}

		// Explore list and recalculate if necessary
		for (int r = 0; r < nodeNum_; r++) {
			// NO_ROUTE_FOUND does not trigger a recalculation
			if (routeTable_.at(terminusNode).at(r).nextHop == NO_ROUTE_FOUND)
				continue;

			bool needRecalculation = false;

			// Empty route condition
			if (routeTable_.at(terminusNode).at(r).nextHop == EMPTY_ROUTE)
				needRecalculation = true;

			// Due route condition
			// todo
			if (routeTable_.at(terminusNode).at(r).toTime <= simTime_)
				needRecalculation = true;

			// Depleted route condition
			if (routeTable_.at(terminusNode).at(r).residualVolume < bundle->getByteLength()) {
				// Make sure that the capacity-limiting contact is marked as depleted
				vector<Contact *>::iterator hop;
				for (hop = routeTable_.at(terminusNode).at(r).hops.begin();
						hop != routeTable_.at(terminusNode).at(r).hops.end(); ++hop)
					if (routeTable_.at(terminusNode).at(r).residualVolume == (*hop)->getResidualVolume())
						(*hop)->setResidualVolume(0);

				needRecalculation = true;
			}

			if (needRecalculation) {

				// Suppress all contacts which connect his node with other nodes
				// different than the entry node of this route table entry (r)
				vector<int> suppressedContactIds;
				for (vector<Contact>::iterator it = contactPlan_->getContacts()->begin();
						it != contactPlan_->getContacts()->end(); ++it)
					if ((*it).getSourceEid() == eid_ && (*it).getDestinationEid() != r)
						suppressedContactIds.push_back((*it).getId());

				CgrRoute route;
				this->findNextBestRoute(suppressedContactIds, terminusNode, &route);
				routeTable_.at(terminusNode).at(r) = route;

				if (route.nextHop != NO_ROUTE_FOUND)
					tableEntriesCreated++;
			}
		}
	}

	// At this point routeTable must be updated
	// no matter which routingType was chosen

	// Print route table for this terminus
	this->printRouteTable(terminusNode);

	// Filter routes
	for (unsigned int r = 0; r < routeTable_.at(terminusNode).size(); r++) {

		routeTable_.at(terminusNode).at(r).filtered = false;

		// Filter those that should not be considered in
		// the next best route determination calculation.
		// Should have no effect when using dynamically updated
		// route tables (perNeighborBestPath and oneBestPath),
		// but on allPaths routing types.

		// criteria 1) filter route: capacity is depleted
		if (routeTable_.at(terminusNode).at(r).residualVolume < bundle->getByteLength()) {
			routeTable_.at(terminusNode).at(r).filtered = true;
			cout << "setting filtered true due to capacity to route next hop = "
					<< routeTable_.at(terminusNode).at(r).nextHop << endl;
		}
		// criteria 2) filter route: due time is passed
		if (routeTable_.at(terminusNode).at(r).toTime <= simTime_) {
			routeTable_.at(terminusNode).at(r).filtered = true;
			cout << "setting filtered true due to time to route next hop = "
					<< routeTable_.at(terminusNode).at(r).nextHop << endl;
		}

		// Filter those that goes back to sender if such
		// type of forwarding is forbidden as per .ini file
		if (bundle->getReturnToSender() == false)
			if (routeTable_.at(terminusNode).at(r).nextHop == bundle->getSenderEid()) {
				routeTable_.at(terminusNode).at(r).filtered = true;
				cout << "setting filtered true due to not-return-to-sender to route next hop = "
						<< routeTable_.at(terminusNode).at(r).nextHop << endl;
			}
	}

	if (!routeTable_.at(terminusNode).empty()) {
		// Select best route
		vector<CgrRoute>::iterator bestRoute;
		bestRoute = min_element(routeTable_.at(terminusNode).begin(), routeTable_.at(terminusNode).end(),
				this->compareRoutes);

		// Save tableEntriesExplored metric. Notice that
		// explored also includes filtered routes (i.e., pessimistic)
		tableEntriesExplored = routeTable_.at(terminusNode).size();

		// Enqueue bundle to route and update volumes
		this->cgrEnqueue(bundle, &(*bestRoute));
	} else {
		// Enqueue to limbo
		bundle->setNextHopEid(NO_ROUTE_FOUND);
		sdr_->enqueueBundleToContact(bundle, 0);

		cout << "*BestRoute not found (enqueing to limbo)" << endl;
		bundlesInLimbo++;
	}
}

// This function enqueues the bundle in the best found path.
// To this end, it updates contacts volume depending on the volume-awareness
// type configured for the routing routine.
void RoutingCgrModelRev17::cgrEnqueue(BundlePkt * bundle, CgrRoute *bestRoute) {

	if (bestRoute->nextHop != NO_ROUTE_FOUND && !bestRoute->filtered) {

		cout << "*Best: routeTable[" << bestRoute->terminusNode << "][ ]: nextHop: " << bestRoute->nextHop << ", frm "
				<< bestRoute->fromTime << " to " << bestRoute->toTime << ", arrival time: " << bestRoute->arrivalTime
				<< ", volume: " << bestRoute->residualVolume << "/" << bestRoute->maxVolume << "Bytes" << endl;

		// When using global contact plan, only the source node
		// must update the global contact plan contact volumes.
		// Otherwise a same path might be reserved several times
		// When using local contact plan, capacity need to
		// be updated every time a bundle is forwarded.
		if (routingType_.find("contactPlan:local") != std::string::npos
				|| (routingType_.find("contactPlan:global") != std::string::npos && bundle->getSourceEid() == eid_)) {

			//////////////////////////////////////////////////
			// Update residualVolume: all contact
			//////////////////////////////////////////////////
			if (routingType_.find("volumeAware:allContacts") != std::string::npos) {
				// Update residualVolume of all this route hops
				for (vector<Contact *>::iterator hop = bestRoute->hops.begin(); hop != bestRoute->hops.end(); ++hop) {
					(*hop)->setResidualVolume((*hop)->getResidualVolume() - bundle->getByteLength());

					// This should never happen. We'ĺl temporarily leave
					// this exit code 1 here to detect potential issues
					// with the volume booking algorithms
					if ((*hop)->getResidualVolume() < 0)
						exit(1);
				}

				// Update residualVolume of all routes that uses the updated hops (including those
				// routes that leads to other destinations). This is a very expensive routine
				// that scales with large routes tables that need to happen in forwarding time.
				for (int n = 1; n < nodeNum_; n++)
					if (!routeTable_.at(n).empty())
						for (unsigned int r = 0; r < routeTable_.at(n).size(); r++)
							for (vector<Contact *>::iterator hop1 = routeTable_.at(n).at(r).hops.begin();
									hop1 != routeTable_.at(n).at(r).hops.end(); ++hop1)
								for (vector<Contact *>::iterator hop2 = bestRoute->hops.begin();
										hop2 != bestRoute->hops.end(); ++hop2)
									if ((*hop1)->getId() == (*hop2)->getId())
										// Does the reduction of this contact volume requires a route volume update?
										if (routeTable_.at(n).at(r).residualVolume > (*hop1)->getResidualVolume()) {
											routeTable_.at(n).at(r).residualVolume = (*hop1)->getResidualVolume();
											cout << "*Rvol: routeTable[" << n << "][" << r << "]: updated to "
													<< (*hop1)->getResidualVolume() << "Bytes (all contacts)" << endl;
										}
			}

			//////////////////////////////////////////////////
			// Update residualVolume: 1st contact
			//////////////////////////////////////////////////
			if (routingType_.find("volumeAware:1stContact") != std::string::npos) {
				// Update residualVolume of the first hop
				bestRoute->hops[0]->setResidualVolume(
						bestRoute->hops[0]->getResidualVolume() - bundle->getByteLength());

				// Update residualVolume of all routes that uses the updated hops (including those
				// routes that leads to other destinations). This is a very expensive routine
				// that scales with large routes tables that need to happen in forwarding time.
				for (int n = 1; n < nodeNum_; n++)
					if (!routeTable_.at(n).empty())
						for (unsigned int r = 0; r < routeTable_.at(n).size(); r++)
							for (vector<Contact *>::iterator hop1 = routeTable_.at(n).at(r).hops.begin();
									hop1 != routeTable_.at(n).at(r).hops.end(); ++hop1)
								for (vector<Contact *>::iterator hop2 = bestRoute->hops.begin();
										hop2 != bestRoute->hops.end(); ++hop2)
									if ((*hop1)->getId() == (*hop2)->getId())
										// Does the reduction of this contact volume requires a route volume update?
										if (routeTable_.at(n).at(r).residualVolume > (*hop1)->getResidualVolume()) {
											routeTable_.at(n).at(r).residualVolume = (*hop1)->getResidualVolume();
											cout << "*Rvol: routeTable[" << n << "][" << r << "]: updated to "
													<< (*hop1)->getResidualVolume() << "Bytes (1st contact)" << endl;
										}
			}

			//////////////////////////////////////////////////
			// Update residualVolume: off -> do nothing
			//////////////////////////////////////////////////
		}

		// Save CgrRoute in header
		if (routingType_.find("extensionBlock:on") != std::string::npos)
			bundle->setCgrRoute(*bestRoute);

		// Enqueue bundle
		cout << "queuing bundle in contact " << bestRoute->hops.at(0)->getId() << endl;

		bundle->setNextHopEid(bestRoute->nextHop);
		sdr_->enqueueBundleToContact(bundle, bestRoute->hops.at(0)->getId());
	} else {
		// Enqueue to limbo
		bundle->setNextHopEid(bestRoute->nextHop);
		sdr_->enqueueBundleToContact(bundle, 0);

		cout << "!*BestRoute not found (enqueing to limbo)" << endl;
		bundlesInLimbo++;
	}
}

// This function is the Dijkstra search over the network graph, not the contact graph.
void RoutingCgrModelRev17::findNextBestRoute(vector<int> suppressedContactIds, int terminusNode, CgrRoute * route) {
	// increment metrics counter
	dijkstraCalls++;

	// Suppress edges
	for (auto& contact : *contactPlan_->getContacts()) {
		contact.work = new Work();
		auto it = std::find(suppressedContactIds.begin(), suppressedContactIds.end(), contact.getId());
		((Work *) contact.work)->suppressed = it != suppressedContactIds.end();
	}

	// Initialize Dijkstra
	double arrivalTimes[nodeNum_ + 1];
	bool visitedNodes[nodeNum_ + 1];
	Contact * predecesors[nodeNum_ + 1];
	for (int i = 1; i <= nodeNum_; i++) {
		arrivalTimes[i] = std::numeric_limits<double>::max();
		visitedNodes[i] = false;
		predecesors[i] = NULL;
	}
	arrivalTimes[eid_] = simTime_;

	// Queue for queueing (arrival time, node) in order to get best node first
	// Use std::greater to sort arrival time from low to high
	priority_queue<pair<double, int>, vector<pair<double, int>>, std::greater<pair<double, int>>> pq;
	pq.push(make_pair(arrivalTimes[eid_], eid_));

	// Start Dijkstra
	while (!pq.empty() && pq.top().second != terminusNode) {
		dijkstraLoops++;

		double arrivalTime = pq.top().first;
		int currentNode = pq.top().second;
		pq.pop();

		if (visitedNodes[currentNode])
			continue;
		visitedNodes[currentNode] = true;

		// Explore current node's neighbors
		for (auto& edgeId : *contactPlan_->getContactIdsBySrc(currentNode)) {
			Contact * edgeToNeighbor = contactPlan_->getContactById(edgeId);
			int neighbor = edgeToNeighbor->getDestinationEid();

			if (visitedNodes[neighbor])
				continue;

			// If edge is suppressed or has already ended, ignore
			if (((Work *) edgeToNeighbor->work)->suppressed  || edgeToNeighbor->getEnd() <= arrivalTime)
				continue;

			double owlt = edgeToNeighbor->getRange();
			double newArrivalTime = max(arrivalTime, edgeToNeighbor->getStart());
			newArrivalTime += owlt;

			if (newArrivalTime < arrivalTimes[neighbor]) {
				arrivalTimes[neighbor] = newArrivalTime;
				predecesors[neighbor] = edgeToNeighbor;

				pq.push(make_pair(newArrivalTime, neighbor));
			}
		}
	}
	// End Dijkstra

	// If we got a final contact to destination
	// then it is the best route and we need to
	// recover the route
	if (predecesors[terminusNode]!= NULL) {
		// Add all contacts in the path
		route->hops.clear();
		for (Contact * contact = predecesors[terminusNode]; contact != NULL; contact =
				predecesors[contact->getSourceEid()]) {
			route->hops.push_back(contact);
		}
		std::reverse(route->hops.begin(), route->hops.end());

		// Set route metrics
		route->filtered = false;
		route->terminusNode = terminusNode;
		route->arrivalTime = arrivalTimes[terminusNode];
		route->confidence = 1.0;
		route->nextHop = route->hops[0]->getDestinationEid();
		route->fromTime = route->hops[0]->getStart();
		route->maxVolume = numeric_limits<double>::max();
		route->residualVolume = numeric_limits<double>::max();
		route->toTime = numeric_limits<double>::max();
		double accumulatedRange = 0;
		for (Contact * contact : route->hops) {
			route->toTime = std::min(route->toTime,
					contact->getEnd() - accumulatedRange);
			accumulatedRange += contact->getRange();

			route->maxVolume = std::min(route->maxVolume, contact->getVolume());
			route->residualVolume = std::min(route->residualVolume, contact->getResidualVolume());
			route->confidence *= contact->getConfidence();
		}
	} else {
		// No route found
		route->terminusNode = NO_ROUTE_FOUND;
		route->nextHop = NO_ROUTE_FOUND;
		route->arrivalTime = numeric_limits<double>::max();	// never chosen as best route
	}

	// Delete working area in each contact.
	for (auto& contact: *contactPlan_->getContacts()) {
		delete ((Work *) contact.work);
	}
}

/////////////////////////////////////////////////
// Auxiliary Functions
/////////////////////////////////////////////////

// This function set all route list to size 0
// (like the initial condition of the class)
void RoutingCgrModelRev17::clearRouteTable() {

	for (int n = 0; n < nodeNum_; n++) {
		routeTable_.at(n).clear();
	}
}

void RoutingCgrModelRev17::printRouteTable(int terminusNode) {
	// Print route table for this destination
	for (unsigned int r = 0; r < routeTable_.at(terminusNode).size(); r++) {
		CgrRoute route = routeTable_.at(terminusNode).at(r);
		if (route.nextHop == NO_ROUTE_FOUND)
			cout << "routeTable[" << terminusNode << "][" << r << "]: No route found" << endl;
		else if (route.nextHop == EMPTY_ROUTE) // should never happen
			cout << "routeTable[" << terminusNode << "][" << r << "]: Need to recalculate route" << endl;
		else {
			cout << "routeTable[" << terminusNode << "][" << r << "]: nextHop: " << route.nextHop << ", frm "
					<< route.fromTime << " to " << route.toTime << ", arrival time: " << route.arrivalTime
					<< ", volume: " << route.residualVolume << "/" << route.maxVolume << "Bytes: ";

			// print route:
			for (vector<Contact *>::iterator hop = route.hops.begin(); hop != route.hops.end(); ++hop)
				cout << "(+" << (*hop)->getStart() << "+" << (*hop)->getEnd() << " " << (*hop)->getSourceEid() << " "
						<< (*hop)->getDestinationEid() << " " << (*hop)->getResidualVolume() << "/"
						<< (*hop)->getVolume() << "Bytes)" << ", toTime: " << route.toTime;

			cout << endl;
		}
	}
}

// This functions is used to determine the best route out of a route list.
// Must returns true if first argument is better (i.e., minor)
bool RoutingCgrModelRev17::compareRoutes(CgrRoute i, CgrRoute j) {

	// If one is filtered, the other is the best
	if (i.filtered && !j.filtered)
		return false;
	if (!i.filtered && j.filtered)
		return true;

	// If both are not filtered, then compare criteria,
	// If both are filtered, return any of them.

	// criteria 1) lowest arrival time
	if (i.arrivalTime < j.arrivalTime)
		return true;
	else if (i.arrivalTime > j.arrivalTime)
		return false;
	else {
		// if equal, criteria 2) lowest hop count
		if (i.hops.size() < j.hops.size())
			return true;
		else if (i.hops.size() > j.hops.size())
			return false;
		else {
			// if equal, criteria 3) larger residual volume
			if (i.residualVolume > j.residualVolume)
				return true;
			else if (i.residualVolume < j.residualVolume)
				return false;
			else {
				// if equal, first is better.
				return true;
			}
		}
	}
}

// Verify the routingType string contains all necessary parameters
void RoutingCgrModelRev17::checkRoutingTypeString() {
	// If no routing type set, set a default one
	if (routingType_.compare("none") == 0) {
		routingType_ = "contactPlan:local,routeListType:perNeighborBestPath,volumeAware:allContacts,extensionBlock:off";
		cout << "NODE: " << eid_ << ", DEFAULT rouingType string: " << routingType_ << endl;
	}

	// Check contactPlan
	if (routingType_.find("contactPlan:local") == std::string::npos
			&& routingType_.find("contactPlan:global") == std::string::npos) {
		cout << "dtnsim error: unknown or missing contactPlan type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check routeListType
	if (routingType_.find("routeListType:allPaths-yen") == std::string::npos
			&& routingType_.find("routeListType:allPaths-initial+anchor") == std::string::npos
			&& routingType_.find("routeListType:allPaths-firstEnding") == std::string::npos
			&& routingType_.find("routeListType:allPaths-firstDepleted") == std::string::npos
			&& routingType_.find("routeListType:oneBestPath") == std::string::npos
			&& routingType_.find("routeListType:perNeighborBestPath") == std::string::npos) {
		cout << "dtnsim error: unknown or missing routeListType type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check volumeAware
	if (routingType_.find("volumeAware:off") == std::string::npos
			&& routingType_.find("volumeAware:1stContact") == std::string::npos
			&& routingType_.find("volumeAware:allContacts") == std::string::npos) {
		cout << "dtnsim error: unknown or missing volumeAware type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	// Check extensionBlock
	if (routingType_.find("extensionBlock:on") == std::string::npos
			&& routingType_.find("extensionBlock:off") == std::string::npos) {
		cout << "dtnsim error: unknown or missing extensionBlock type in routingType string: " << routingType_ << endl;
		exit(1);
	}

	//cout << "NODE: " << eid_ << ", rouingType string: " << routingType_ << endl;
}

//////////////////////
// Stats recollection
//////////////////////

int RoutingCgrModelRev17::getDijkstraCalls() {
	return dijkstraCalls;
}

int RoutingCgrModelRev17::getDijkstraLoops() {
	return dijkstraLoops;
}

int RoutingCgrModelRev17::getRouteTableEntriesCreated() {
	return tableEntriesCreated;
}

int RoutingCgrModelRev17::getRouteTableEntriesExplored() {
	return tableEntriesExplored;
}
int RoutingCgrModelRev17::getBundlesInLimbo() {
	return bundlesInLimbo;
}
