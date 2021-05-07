/*
 * Subject.h
 *
 *  Created on: Jul 25, 2017
 *      Author: FRaverta
 */

#ifndef SRC_UTILS_SUBJECT_H_
#define SRC_UTILS_SUBJECT_H_

#include <forward_list>
#include "Observer.h"
using namespace std;

class Subject
{
public:
	Subject();
	virtual ~Subject();

	void addObserver(Observer * o);
	void removeObserver(Observer * o);

protected:
	void notify();

private:
	forward_list<Observer *> observerList_;
};

#endif /* SRC_UTILS_SUBJECT_H_ */
