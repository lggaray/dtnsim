#include <src/node/dtn/ContactPlan.h>

ContactPlan::~ContactPlan()
{

}

ContactPlan::ContactPlan()
{

}

ContactPlan::ContactPlan(ContactPlan &contactPlan)
{
	this->nextContactId = contactPlan.nextContactId;
	this->lastEditTime = contactPlan.lastEditTime;
	this->contactsFile_ = contactPlan.contactsFile_;
	this->contacts_ = contactPlan.contacts_;
	this->ranges_ = contactPlan.ranges_;
	this->contactIdShift_ = contactPlan.contactIdShift_;
	this->contactIdsBySrc_ = contactPlan.contactIdsBySrc_;

	for (size_t i = 0; i < contacts_.size(); i++)
	{
		contacts_.at(i).work = NULL;
	}

	for (size_t i = 0; i < ranges_.size(); i++)
	{
		ranges_.at(i).work = NULL;
	}
}

void ContactPlan::parseContactPlanFile(string fileName, int nodesNumber)
{
	this->contactIdsBySrc_.resize(nodesNumber + 1);

	double start = 0.0;
	double end = 0.0;
	int sourceEid = 0;
	int destinationEid = 0;
	double dataRateOrRange = 0.0;

	string fileLine = "#";
	string a;
	string command;
	ifstream file;

	file.open(fileName.c_str());

	if (!file.is_open())
		throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());

	while (getline(file, fileLine))
	{
		if (fileLine.empty())
			continue;

		if (fileLine.at(0) == '#')
			continue;

		// This seems to be a valid command line, parse it
		stringstream stringLine(fileLine);
		stringLine >> a >> command >> start >> end >> sourceEid >> destinationEid >> dataRateOrRange;

		if (a.compare("a") == 0)
		{
			if ((command.compare("contact") == 0))
			{
				this->addContact(start, end, sourceEid, destinationEid, dataRateOrRange, (float) 1.0);
			}
			else if ((command.compare("range") == 0))
			{
				this->addRange(start, end, sourceEid, destinationEid, dataRateOrRange, (float) 1.0);
			}
			else
			{
				cout << "dtnsim error: unknown contact plan command type: a " << fileLine << endl;
			}
		}
	}

	if (cin.bad())
	{
		// IO error
	}
	else if (!cin.eof())
	{
		// format error (not possible with getline but possible with operator>>)
	}
	else
	{
		// format error (not possible with getline but possible with operator>>)
		// or end of file (can't make the difference)
	}

	file.close();

	this->setContactsFile(fileName);
	this->updateContactRanges();
	this->sortContactIdsBySrcByStartTime();
}

int ContactPlan::addContact(double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence)
{
	int id = this->nextContactId++;
	Contact contact(id, start, end, sourceEid, destinationEid, dataRate, confidence, 0);

	contactIdShift_.push_back(id - contacts_.size());
	contacts_.push_back(contact);

	contactIdsBySrc_[sourceEid].push_back(id);

	lastEditTime = simTime();
	return id;
}

void ContactPlan::addRange(double start, double end, int sourceEid, int destinationEid, double range, float confidence)
{
	// Ranges can be declared in a single direction, but they are bidirectional
	// In the worst case they are repeated
	Contact contact1(-1, start, end, sourceEid, destinationEid, 0, confidence, range);
	Contact contact2(-1, start, end, destinationEid, sourceEid, 0, confidence, range);

	ranges_.push_back(contact1);
	ranges_.push_back(contact2);

	lastEditTime = simTime();
}

double ContactPlan::getRangeBySrcDst(int Src, int Dst)
{

	double rangeFirstContact = -1;

	for (size_t i = 0; i < ranges_.size(); i++)
	{
		if ((ranges_.at(i).getSourceEid() == Src) && (ranges_.at(i).getDestinationEid() == Dst))
		{
			rangeFirstContact = ranges_.at(i).getRange();
		}
	}

	return rangeFirstContact;
}

// Each contact matches its position in the vector with the formula:
// position = id - contactIdShift[id]
Contact *ContactPlan::getContactById(int id)
{
	Contact *contactPtr = NULL;

	if (contactIdShift_.at(id) != DELETED_CONTACT) {
		contactPtr = &contacts_.at(id - contactIdShift_.at(id));
	}

	return contactPtr;
}

vector<Contact> * ContactPlan::getContacts()
{
	return &contacts_;
}

vector<Contact> * ContactPlan::getRanges()
{
	return &ranges_;
}

vector<int> * ContactPlan::getContactIdsBySrc(int Src)
{
	return &contactIdsBySrc_.at(Src);
}

vector<Contact> ContactPlan::getContactsBySrc(int Src)
{
	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if (contacts_.at(i).getSourceEid() == Src)
		{
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

vector<Contact> ContactPlan::getContactsByDst(int Dst)
{
	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if (contacts_.at(i).getDestinationEid() == Dst)
		{
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

vector<Contact> ContactPlan::getContactsBySrcDst(int Src, int Dst)
{
	vector<Contact> contacts;

	for (size_t i = 0; i < contacts_.size(); i++)
	{
		if ((contacts_.at(i).getSourceEid() == Src) && (contacts_.at(i).getDestinationEid() == Dst))
		{
			contacts.push_back(contacts_.at(i));
		}
	}

	return contacts;
}

simtime_t ContactPlan::getLastEditTime()
{
	return lastEditTime;
}

void ContactPlan::setContactsFile(string contactsFile)
{
	contactsFile_ = contactsFile;
}

const string& ContactPlan::getContactsFile() const
{
	return contactsFile_;
}

void ContactPlan::printContactPlan()
{
	vector<Contact>::iterator it;
	for (it = this->getContacts()->begin(); it != this->getContacts()->end(); ++it)
		cout << "a contact +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getResidualVolume() << "/" << (*it).getVolume() << endl;

	for (it = this->getRanges()->begin(); it != this->getRanges()->end(); ++it)
			cout << "a range +" << (*it).getStart() << " +" << (*it).getEnd() << " " << (*it).getSourceEid() << " " << (*it).getDestinationEid() << " " << (*it).getRange() << endl;
	cout << endl;
}

vector<Contact>::iterator ContactPlan::deleteContactById(int contactId)
{
	vector<Contact>::iterator itReturn;

	if (contactIdShift_.at(contactId) == DELETED_CONTACT) {
		cout << "Warning: Trying to delete a contact already deleted" << endl;
		return itReturn;
	}

	// Update contactIdsBySrc
	int src = getContactById(contactId)->getSourceEid();
	auto it = std::find(contactIdsBySrc_[src].begin(), contactIdsBySrc_[src].end(), contactId);
	if (it != contactIdsBySrc_[src].end())
		contactIdsBySrc_[src].erase(it);

	// Erase contact
	int contactIndex = contactId - contactIdShift_.at(contactId);
	itReturn = contacts_.erase(contacts_.begin() + contactIndex);

	// Update shift indices
	contactIdShift_.at(contactId) = DELETED_CONTACT;
	for (size_t shiftIndex = contactId + 1; shiftIndex < contactIdShift_.size(); shiftIndex++) {
		if (contactIdShift_[shiftIndex] != DELETED_CONTACT)
			contactIdShift_[shiftIndex]++;
	}

	return itReturn;
}

void ContactPlan::updateContactRanges()
{
	for (auto& rangeContact : ranges_) {
		int sourceEid = rangeContact.getSourceEid();
		int destinationEid = rangeContact.getDestinationEid();
		double start = rangeContact.getStart();
		double end = rangeContact.getEnd();

		for (auto& contactId : *getContactIdsBySrc(sourceEid)) {
			Contact* contact = getContactById(contactId);
			if (contact->getDestinationEid() == destinationEid &&
					contact->getStart() >= start &&
					contact->getEnd() <= end) {

				contact->setRange(rangeContact.getRange());
			}
		}
	}
}

void ContactPlan::sortContactIdsBySrcByStartTime()
{
	for (auto& vectorOfIds : this->contactIdsBySrc_) {
		std::sort(vectorOfIds.begin(), vectorOfIds.end(),
			[this](int id1, int id2) {
				return this->getContactById(id1)->getStart() <
						this->getContactById(id2)->getStart();
			}
		);
	}
}
