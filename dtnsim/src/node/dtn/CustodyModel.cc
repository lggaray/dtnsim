#include "src/node/dtn/CustodyModel.h"

CustodyModel::CustodyModel()
{
}

CustodyModel::~CustodyModel()
{
}

void CustodyModel::setEid(int eid)
{
	this->eid_ = eid;
}

void CustodyModel::setSdr(SdrModel * sdr)
{
	this->sdr_ = sdr;
}

void CustodyModel::setCustodyReportByteSize(int custodyReportByteSize)
{
	this->custodyReportByteSize_ = custodyReportByteSize;
}

// Bundle with custody flag arrived to this node or in-transit
BundlePkt * CustodyModel::bundleWithCustodyRequestedArrived(BundlePkt * bundleInCustody)
{
	BundlePkt * custodyReport;

	if (sdr_->isSdrFreeSpace(bundleInCustody->getByteLength()) || this->eid_ == bundleInCustody->getDestinationEid())
	{
		// Accept custody, send custody report to previous custodian
		cout << simTime() << " Node " << eid_ << " ** Custody accept for bundleId " << bundleInCustody->getBundleId() << " - ";
		custodyReport = this->getNewCustodyReport(true, bundleInCustody);
		bundleInCustody->setCustodianEid(eid_);
	}
	else
	{
		// Reject custody, send custody report to previous custodian
		cout << simTime() << " Node " << eid_ << " ** Custody reject for bundleId " << bundleInCustody->getBundleId() << " - ";
		custodyReport = this->getNewCustodyReport(false, bundleInCustody);
	}

	return custodyReport;
}

// Custody Report arrived to this node. Function return the bundle held in custody
// in case the custody was rejected. This bundle should be forwarded again later.
BundlePkt * CustodyModel::custodyReportArrived(BundlePkt * custodyReport)
{
	if (custodyReport->getSourceEid() == eid_)
	{
		// I sent this report to myself, meaning I was the generator of the bundle in custody
		delete custodyReport;
		return NULL;
	}

	BundlePkt * reSendBundle = NULL;

	if (custodyReport->getCustodyAccepted())
	{
		// Custody was accepted by remote node, release custodyBundleId
		cout << simTime() << " Node " << eid_ << " ** Releasing custody of bundleId " << custodyReport->getCustodyBundleId() << endl;
		sdr_->removeTransmittedBundleInCustody(custodyReport->getCustodyBundleId());
	}
	else
	{
		// Custody was rejected by remote node, return a pointer to resend the bundle
		cout << simTime() << " Node " << eid_ << " ** NOT releasing custody of bundleId " << custodyReport->getCustodyBundleId() << " rejected... do something!" << endl;
		reSendBundle = sdr_->getTransmittedBundleInCustody(custodyReport->getCustodyBundleId())->dup();
		sdr_->removeTransmittedBundleInCustody(custodyReport->getCustodyBundleId());

		// TODO: add custody node as forbidden neighbor and reroute custodyBundleId.
		// Also, a mechanism to remove node from forbidden list must be implemented.
	}
	delete custodyReport;

	return reSendBundle;
}

// The custody timer expired, we have to check if this bundle still exits in the custody
// memory and if that is the case, return a pointer to a copy for retransmission.
BundlePkt * CustodyModel::custodyTimerExpired(CustodyTimout * custodyTimout){

	// Get bundle from custody Sdr if any
	BundlePkt * reSendBundle = sdr_->getTransmittedBundleInCustody(custodyTimout->getBundleId());

	// If still in memory, remove it from Sdr and transmit a copy of it
	if (reSendBundle != NULL)
	{
		cout << simTime() << " Node " << eid_ << " ** Resending bundleId " << custodyTimout->getBundleId() << endl;

		reSendBundle = reSendBundle->dup();
		sdr_->removeTransmittedBundleInCustody(custodyTimout->getBundleId());
	}

	this->printBundlesInCustody();
	return reSendBundle;
}

/////////////////////////////
// Private Methods
/////////////////////////////

BundlePkt * CustodyModel::getNewCustodyReport(bool accept, BundlePkt *bundleInCustody)
{
	BundlePkt* custodyReport = new BundlePkt("custodyReport", BUNDLE_CUSTODY_REPORT);
	custodyReport->setSchedulingPriority(BUNDLE_CUSTODY_REPORT);

	// Bundle properties
	char bundleName[100];
	sprintf(bundleName, "Src:%d,Dst:%d(id:%d)", this->eid_, bundleInCustody->getCustodianEid(), (int) custodyReport->getId());
	custodyReport->setBundleId(custodyReport->getId());
	custodyReport->setName(bundleName);
	custodyReport->setBitLength(custodyReportByteSize_ * 8);
	custodyReport->setByteLength(custodyReportByteSize_);

	// Bundle fields (set by source node)
	custodyReport->setSourceEid(this->eid_);
	custodyReport->setDestinationEid(bundleInCustody->getCustodianEid());
	custodyReport->setReturnToSender(false);
	custodyReport->setCritical(false);
	custodyReport->setCustodyTransferRequested(false);
	custodyReport->setTtl(9000000);
	custodyReport->setCreationTimestamp(simTime());
	custodyReport->setQos(2);

	// Custody fields
	custodyReport->setBundleIsCustodyReport(true);
	custodyReport->setCustodyBundleId(bundleInCustody->getBundleId());
	custodyReport->setCustodyAccepted(accept);

	// Bundle meta-data (set by intermediate nodes)
	custodyReport->setHopCount(0);
	custodyReport->setNextHopEid(0);
	custodyReport->setSenderEid(0);
	custodyReport->setCustodianEid(0);
	custodyReport->getVisitedNodes().clear();
	CgrRoute emptyRoute;
	emptyRoute.nextHop = EMPTY_ROUTE;
	custodyReport->setCgrRoute(emptyRoute);

	cout << "Sending report: " << bundleName << endl;

	return custodyReport;
}

void CustodyModel::printBundlesInCustody(){

	list<BundlePkt *> transmittedBundlesInCustody_ = sdr_->getTransmittedBundlesInCustody();

	cout <<  "Node " << eid_ << " Bundles in custody: ";
	for (list<BundlePkt *>::iterator it = transmittedBundlesInCustody_.begin(); it != transmittedBundlesInCustody_.end(); it++)
		cout << (*it)->getBundleId() << ",";
	cout << endl;
}
