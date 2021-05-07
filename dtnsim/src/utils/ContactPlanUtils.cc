#include "ContactPlanUtils.h"

namespace contactPlanUtils
{

vector<Contact> getDifferenceContacts(ContactPlan &contactPlan, ContactPlan &contactTopology)
{
	vector<Contact> contacts;

	vector<Contact> *cpContacts = contactPlan.getContacts();

	for(size_t i = 0; i<cpContacts->size(); i++)
	{
		int cpContactId = cpContacts->at(i).getId();

		if(contactTopology.getContactById(cpContactId) == NULL)
		{
			contacts.push_back(cpContacts->at(i));
		}
	}

	return contacts;
}

}

