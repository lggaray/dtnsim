#ifndef CONTACTPLAN_H_
#define CONTACTPLAN_H_

#include <src/node/dtn/Contact.h>
#include <omnetpp.h>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;
using namespace omnetpp;

class ContactPlan {

public:

	ContactPlan();
	virtual ~ContactPlan();
	ContactPlan(ContactPlan &contactPlan);

	// Contact plan population functions
	int addContact(double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence);
	void addRange(double start, double end, int sourceEid, int destinationEid, double range, float confidence);

	// Contact plan exploration functions
	Contact *getContactById(int id);
	vector<Contact> * getContacts();
	vector<Contact> * getRanges();
	vector<Contact> getContactsBySrc(int Src);
	vector<int> * getContactIdsBySrc(int Src);
	vector<Contact> getContactsByDst(int Dst);
	vector<Contact> getContactsBySrcDst(int Src, int Dst);
	double getRangeBySrcDst(int Src, int Dst);
	void parseContactPlanFile(string fileName, int nodesNumber);
	void setContactsFile(string contactsFile);
	const string& getContactsFile() const;
	simtime_t getLastEditTime();

	// delete contact function
	vector<Contact>::iterator deleteContactById(int contactId);

	// debug function
	void printContactPlan();

private:

	void updateContactRanges();
	void sortContactIdsBySrcByStartTime();

	static const int DELETED_CONTACT = -1;
	int nextContactId = 1;
	vector<Contact> contacts_;
	vector<Contact> ranges_;
	vector<vector<int>> contactIdsBySrc_;
	vector<int> contactIdShift_ = {0}; // create dummy element for limbo contact
	simtime_t lastEditTime;
	string contactsFile_;

};

#endif /* CONTACTPLAN_H_ */
