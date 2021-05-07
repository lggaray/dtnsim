#include <src/utils/Subject.h>

Subject::Subject()
{
}

Subject::~Subject()
{
}

void Subject::addObserver(Observer * o)
{
	observerList_.push_front(o);
}

void Subject::removeObserver(Observer * o)
{
	observerList_.remove(o);
}


void Subject::notify()
{
	for( forward_list<Observer *>::iterator it = observerList_.begin(); it != observerList_.end(); it++)
		(*it)->update();
}
