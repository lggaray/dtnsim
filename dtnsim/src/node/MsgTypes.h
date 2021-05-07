#ifndef SRC_NODE_MSGTYPES_H_
#define SRC_NODE_MSGTYPES_H_

// Messages types and their priority are
// equal, unique and defined in this file.
// Higher scheduling priority (smaller
// numeric value) are executed first.

#define CONTACT_END_TIMER 0
#define CONTACT_START_TIMER 1

#define TRAFFIC_TIMER 2

#define BUNDLE_CUSTODY_REPORT 3	// white
#define BUNDLE 4				// yellow

#define CUSTODY_TIMEOUT 5

#define FORWARDING_MSG_END 6
#define FORWARDING_MSG_START 7

#define FAULT_END_TIMER 21
#define FAULT_START_TIMER 20

#endif /* SRC_NODE_MSGTYPES_H_ */
