#ifdef USE_BOOST_LIBRARIES

#ifndef TOPOLOGYGRAPHINFO_H_
#define TOPOLOGYGRAPHINFO_H_

struct TopologyVertexInfo
{
    // vertex id
	int eid;
};


struct TopologyEdgeInfo
{
    // edge id
	int id;

	// state capacity in bytes
	double stateCapacity;
};

struct TopologyGraphInfo
{
	// state start
	double stateStart;

	// state end
	double stateEnd;
};

#endif /* TOPOLOGYGRAPHINFO_H_ */

#endif /* USE_BOOST_LIBRARIES */
