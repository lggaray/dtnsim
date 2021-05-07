#include "Fault.h"

Define_Module (Fault);

void Fault::initialize()
{
	// Store this node eid
	this->eid_ = this->getParentModule()->getIndex();

	// Get a pointer to graphics module
	graphicsModule = (Graphics *) this->getParentModule()->getSubmodule("graphics");

	// Get a pointer to dtn module
	dtnModule = (Dtn *) this->getParentModule()->getSubmodule("dtn");

	// Initialize faults
	if ((this->par("enable").boolValue() == true) && eid_ != 0)
	{
		meanTTF = this->par("meanTTF").doubleValue();
		meanTTR = this->par("meanTTR").doubleValue();

		cMessage *faultMsg = new ContactMsg("fault", FAULT_START_TIMER);
		scheduleAt(exponential(meanTTF), faultMsg);
	}
}

void Fault::handleMessage(cMessage *msg)
{
	if (msg->getKind() == FAULT_START_TIMER)
	{
		// Enable fault mode
		graphicsModule->setFaultOn();
		dtnModule->setOnFault(true);

		// Schedule fault recovery
		msg->setKind(FAULT_END_TIMER);
		scheduleAt(simTime() + exponential(meanTTR), msg);
	}
	else if (msg->getKind() == FAULT_END_TIMER)
	{
		// Disable fault mode
		graphicsModule->setFaultOff();
		dtnModule->setOnFault(false);

		// Schedule next fault
		msg->setKind(FAULT_START_TIMER);
		scheduleAt(simTime() + exponential(meanTTF), msg);
	}
}
