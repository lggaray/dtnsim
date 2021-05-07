/*
 * SdrModel.h
 *
 *  Created on: Nov 25, 2016
 *      Author: juanfraire
 */

#ifndef SRC_NODE_DTN_SDRMODEL_H_
#define SRC_NODE_DTN_SDRMODEL_H_

#include <src/node/dtn/ContactPlan.h>
#include <src/node/dtn/SdrStatus.h>
#include <map>
#include <omnetpp.h>
#include "src/utils/Subject.h"

#include "src/dtnsim_m.h"
#include "assert.h"

using namespace omnetpp;
using namespace std;

class SdrModel: public Subject
{
public:
	SdrModel();
	virtual ~SdrModel();

	// Initialization and configuration
	virtual void setEid(int eid);
	virtual void setNodesNumber(int nodesNumber);
	virtual void setContactPlan(ContactPlan *contactPlan);
	virtual void setSize(int size);
	virtual void freeSdr(int eid);

	// Get information
	virtual int getBundlesCountInSdr();
	virtual int getBundlesCountInContact(int cid);
	virtual int getBundlesCountInLimbo();
	virtual list<BundlePkt*> * getBundlesInLimbo();
	virtual int getBytesStoredInSdr();
	virtual int getBytesStoredToNeighbor(int eid);
	virtual SdrStatus getSdrStatus();
	virtual BundlePkt * getEnqueuedBundle(long bundleId);
	bool isSdrFreeSpace(int sizeNewPacket);
	virtual int getBundlesDeletedInSdr();

	// Enqueue and dequeue from perContactBundleQueue_
	virtual bool enqueueBundleToContact(BundlePkt * bundle, int contactId);
	virtual bool isBundleForContact(int contactId);
	virtual BundlePkt * getNextBundleForContact(int contactId);
	virtual void popNextBundleForContact(int contactId);

	// Enqueue and dequeue from genericBundleQueue_
	virtual bool enqueueBundle(BundlePkt * bundle);
	virtual void removeBundle(long bundleId);
	virtual list<BundlePkt *> getCarryingBundles();

	// Enqueue and dequeue from transmittedBundlesInCustody_
	virtual bool enqueueTransmittedBundleInCustody(BundlePkt * bundle);
	virtual void removeTransmittedBundleInCustody(long bundleId);
	virtual BundlePkt * getTransmittedBundleInCustody(long bundleId);
	virtual list<BundlePkt *> getTransmittedBundlesInCustody();

private:

	int size_;  		// Capacity of sdr in bytes
	int eid_;  			// Local eid of the node
	int nodesNumber_;	// Number of nodes in the network
	int bytesStored_;	// Total Bytes stored in Sdr
	int bundlesNumber_;	// Total bundles enqueued in all sdr queues (index, generic, in custody)
	int bundlesDeleted_; // Total of bundles deleted due to lack of buffer space

	ContactPlan *contactPlan_;

	// Indexed queues where index can be used by routing algorithms
	// to enqueue bundles to specific contacts or nodes. When there
	// is no need for an indexed queue, a generic one can be used instead
	map<int, list<BundlePkt *> > perContactBundleQueue_;
	map<int, list<BundlePkt *> > perNodeBundleQueue_;
	list<BundlePkt *> genericBundleQueue_;

	// A separate area of memory to store transmitted bundles for which
	// the current node is custodian. Bundles are removed as custody reports
	// arrives with either custody acceptance or rejection of a remote node
	list<BundlePkt *> transmittedBundlesInCustody_;



};


#endif /* SRC_NODE_DTN_SDRMODEL_H_ */
