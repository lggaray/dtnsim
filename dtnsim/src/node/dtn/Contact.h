#ifndef CONTACT_H_
#define CONTACT_H_

#include<iostream>

using namespace std;

class Contact
{
public:

	Contact(int id, double start, double end, int sourceEid, int destinationEid, double dataRate, float confidence, double range);
	virtual ~Contact();

	// A contact Id (unique)
	int getId() const;

	// Get basic parameters
	double getStart() const;
	double getEnd() const;
	int getSourceEid() const;
	int getDestinationEid() const;
	double getDataRate() const;
	double getVolume() const;
	double getDuration() const;
	float getConfidence() const;
	double getRange() const;
	void setRange(double range);

	// Get and Set residual capacity (Bytes)
	double getResidualVolume() const;
	void setResidualVolume(double residualVolume);

	// A pointer to external structures
	// (used by routing algorithms)
	void * work;

private:

	int id_;
	double start_;
	double end_;
	int sourceEid_;
	int destinationEid_;
	double dataRate_; // In Bytes per seconds
	double residualVolume_; // In Bytes
	float confidence_;
	double range_;
};

#endif /* CONTACT_H_ */
