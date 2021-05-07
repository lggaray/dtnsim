#ifndef COM_H_
#define COM_H_

#include <src/node/dtn/ContactPlan.h>

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>
#include <fstream>
#include <iomanip>

#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

using namespace std;
using namespace omnetpp;

class Com: public cSimpleModule
{
public:
	Com();
	virtual ~Com();
	virtual void setContactTopology(ContactPlan &contactTopology);

protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

private:

	int eid_;
	ContactPlan contactTopology_;

	double packetLoss_;

};

#endif /* COM_H_ */
