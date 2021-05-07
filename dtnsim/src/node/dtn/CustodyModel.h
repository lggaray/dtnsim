/*
 * CustodyModel.h
 *
 *  Created on: Dec 5, 2017
 *      Author: juanfraire
 */

#ifndef SRC_NODE_DTN_CUSTODYMODEL_H_
#define SRC_NODE_DTN_CUSTODYMODEL_H_

#include <src/node/dtn/SdrModel.h>
#include "src/node/MsgTypes.h"
#include <omnetpp.h>

using namespace omnetpp;

class CustodyModel
{
public:
	CustodyModel();
	virtual ~CustodyModel();

	// Initialization and configuration
	void setEid(int eid);
	void setSdr(SdrModel * sdr);
	void setCustodyReportByteSize(int custodyReportByteSize);

	// Events from Dtn layer
	BundlePkt * bundleWithCustodyRequestedArrived(BundlePkt * bundle);
	BundlePkt * custodyReportArrived(BundlePkt * bundle);
	BundlePkt * custodyTimerExpired(CustodyTimout * custodyTimout);

	void printBundlesInCustody(void);

private:

	BundlePkt * getNewCustodyReport(bool accept, BundlePkt * bundle);

	int eid_;
	SdrModel * sdr_;

	int custodyReportByteSize_;
};

#endif /* SRC_NODE_DTN_CUSTODYMODEL_H_ */
