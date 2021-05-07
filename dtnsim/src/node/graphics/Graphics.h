#ifndef SRC_NODE_GRAPHICS_GRAPHICS_H_
#define SRC_NODE_GRAPHICS_GRAPHICS_H_

#include <omnetpp.h>
#include <vector>
#include <sstream>

#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

using namespace omnetpp;
using namespace std;

class Graphics: public cSimpleModule
{

public:
	Graphics();
	virtual ~Graphics();

	void setFaultOn();
	void setFaultOff();
	void setContactOn(ContactMsg* contactMsg);
	void setContactOff(ContactMsg* contactMsg);
	void setBundlesInSdr(int bundNum);

protected:
	virtual void initialize();
	virtual void finish();

	// Local endpoint id
	int eid_;

	// Number of nodes
	int numNodes;

	// Pointer to parent canvas
	cCanvas *networkCanvas;

	// Pointer to node
	cModule *nodeModule;

	// Visualization
	float posX, posY, posAngle, posRadius;
	vector<cLineFigure *> lines;
};

#endif /* SRC_NODE_GRAPHICS_GRAPHICS_H_ */
