//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __DTNSIM_FAULT_H_
#define __DTNSIM_FAULT_H_

#include "src/node/dtn/Dtn.h"
#include <omnetpp.h>

#include "src/node/graphics/Graphics.h"
#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

using namespace omnetpp;

class Fault: public cSimpleModule
{
protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);

private:

	int eid_;

	double meanTTF, meanTTR;

	// Pointer to grahics module
	Graphics *graphicsModule;

	// Pointer to net module
	Dtn *dtnModule;
};

#endif
