
#ifndef CONTACTPLANUTILS_H_
#define CONTACTPLANUTILS_H_

#include "src/node/dtn/ContactPlan.h"

using namespace std;

namespace contactPlanUtils
{
	/// @brief get the contacts of a contact plan which are not present in a contact topology
	vector<Contact> getDifferenceContacts(ContactPlan &contactPlan, ContactPlan &contactTopology);


}

#endif /* CONTACTPLANUTILS_H_ */


