#ifndef APP_H_
#define APP_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

#include "src/node/MsgTypes.h"
#include "src/dtnsim_m.h"

#include <fstream>
#include <iostream>
using namespace omnetpp;
using namespace std;

class App : public cSimpleModule
{
    public:
        App();
        virtual ~App();
        int getEid() const;
        virtual vector<int> getBundlesNumberVec();
        virtual vector<int> getDestinationEidVec();
        virtual vector<int> getSizeVec();
        virtual vector<double> getStartVec();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *);
        virtual void finish();

    private:
        int eid_;

        std::vector<int> bundlesNumberVec_;
        std::vector<int> destinationEidVec_;
        std::vector<int> sizeVec_;
        std::vector<double> startVec_;

        // Signal
        simsignal_t appBundleSent;
        simsignal_t appBundleReceived;
        simsignal_t appBundleReceivedHops;
        simsignal_t appBundleReceivedDelay;

};

#endif /* APP_H_ */
