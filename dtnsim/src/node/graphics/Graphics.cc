#include "Graphics.h"

Define_Module (Graphics);

int marginX = 160;
int marginY = 40;

void Graphics::initialize()
{
	// Store this node eid
	eid_ = this->getParentModule()->getIndex();

	// Store a pointer to local node module
	nodeModule = this->getParentModule();

	// Store number of neighbors
	numNodes = nodeModule->getVectorSize() - 1;

	// Store a pointer to network canvas
	networkCanvas = nodeModule->getParentModule()->getCanvas();

	if (hasGUI())
	{
		// Set icons
		cDisplayString& dispStr = nodeModule->getDisplayString();
		string icon_path = "device/";
		string icon = nodeModule->par("icon");
		icon_path.append(icon);
		dispStr.setTagArg("i", 0, icon_path.c_str());

		// Set circular position
		posRadius = numNodes * 250 / (2 * (3.1415));
		posAngle = 2 * (3.1415) / ((float) numNodes);
		posX = marginX + posRadius * cos((eid_ - 1) * posAngle) + posRadius;
		posY = marginY + posRadius * sin((eid_ - 1) * posAngle) + posRadius;
		dispStr.setTagArg("p", 0, posX);
		dispStr.setTagArg("p", 1, posY);

		// Extend background area
		nodeModule->getParentModule()->getDisplayString().setTagArg("bgb", 0, 2 * marginX + 2 * posRadius);
		nodeModule->getParentModule()->getDisplayString().setTagArg("bgb", 1, 4 * marginY + 2 * posRadius);

		// Place node 0 away from the network
		if (eid_ == 0)
		{
			nodeModule->getDisplayString().setTagArg("p", 0, 40);
			nodeModule->getDisplayString().setTagArg("p", 1, 100);
			nodeModule->getDisplayString().setTagArg("b", 0, 1);
			nodeModule->getDisplayString().setTagArg("b", 0, 1);
			nodeModule->getDisplayString().setTagArg("i", 0, "old/ball2_vs");
		}
	}
}

void Graphics::setFaultOn()
{
	// Visualize fault start
	if (hasGUI() && this->par("enable"))
	{
		cDisplayString& dispStr = nodeModule->getDisplayString();
		string faultColor = "red";
		dispStr.setTagArg("i", 1, faultColor.c_str());
		dispStr.setTagArg("i2", 0, "status/stop");
	}

}

void Graphics::setFaultOff()
{
	if (hasGUI() && this->par("enable"))
	{
		cDisplayString& dispStr = nodeModule->getDisplayString();
		dispStr.setTagArg("i", 1, "");
		dispStr.setTagArg("i2", 0, "");
	}
}

void Graphics::setContactOn(ContactMsg* contactMsg)
{
	if (hasGUI() && this->par("enable"))
	{
		string lineName = "line";
		lineName.append(to_string(contactMsg->getId()));
		cLineFigure *line = new cLineFigure(lineName.c_str());
		line->setStart(cFigure::Point(posX, posY));
		float endPosX = marginX + posRadius * cos((contactMsg->getDestinationEid() - 1) * posAngle) + posRadius;
		float endPosY = marginY + posRadius * sin((contactMsg->getDestinationEid() - 1) * posAngle) + posRadius;
		line->setEnd(cFigure::Point(endPosX, endPosY));
		line->setLineWidth(2);
		line->setEndArrowhead(cFigure::ARROW_BARBED);
		lines.push_back(line);
		networkCanvas->addFigure(line);
	}
}

void Graphics::setContactOff(ContactMsg* contactMsg)
{
	if (hasGUI() && this->par("enable"))
	{
		string lineName = "line";
		lineName.append(to_string(contactMsg->getId()));

		cFigure* figure = networkCanvas->findFigureRecursively(lineName.c_str());

		if(figure != NULL)
		{
			networkCanvas->removeFigure(figure);
		}
	}
}

void Graphics::setBundlesInSdr(int bundNum)
{
	if (eid_ == 0)
		return;

	stringstream str;
	str << "sdr:" << bundNum;
	nodeModule->getDisplayString().setTagArg("t", 0, str.str().c_str());
}

void Graphics::finish()
{
	// Remove and delete visualization lines
	for (vector<cLineFigure *>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if (networkCanvas->findFigure((*it)) != -1)
			networkCanvas->removeFigure((*it));
		delete (*it);
	}
}

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
}

