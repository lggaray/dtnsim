#include "../utils/TopologyUtils.h"

#ifdef USE_BOOST_LIBRARIES

using namespace std;
using namespace boost;

namespace topologyUtils
{
map<double, TopologyGraph> computeTopology(ContactPlan *contactPlan, int nodesNumber)
{
	map<double, TopologyGraph> topology;

	//  vector with the start times of each topology state
	vector<double> stateTimes;
	stateTimes.push_back(0);
	TopologyGraph topologyGraph;
	topology[0] = topologyGraph;

	vector<Contact> *contacts = contactPlan->getContacts();
	vector<Contact>::iterator it1 = contacts->begin();
	vector<Contact>::iterator it2 = contacts->end();
	for (; it1 != it2; ++it1)
	{
		Contact contact = *it1;

		stateTimes.push_back(contact.getStart());
		stateTimes.push_back(contact.getEnd());

		std::sort(stateTimes.begin(), stateTimes.end());
		stateTimes.erase(std::unique(stateTimes.begin(), stateTimes.end()), stateTimes.end());
	}

	// fill topology with empty topologyGraphs (with vertices, without edges)
	for (size_t i = 0; i<stateTimes.size() -1; i++)
	{
		TopologyGraph topGraph;
		for (int i = 1; i <= nodesNumber; i++)
		{
			TopologyGraph::vertex_descriptor vertex = add_vertex(topGraph);
			topGraph[vertex].eid = i;
		}

		double stateStart = stateTimes.at(i);
		double stateEnd = 0.0;
		++i;
		stateEnd = stateTimes.at(i);
		--i;

		topGraph[graph_bundle].stateStart = stateStart;
		topGraph[graph_bundle].stateEnd = stateEnd;
		topology[stateStart] = topGraph;
	}

	// traversing contacts to add corresponding edges in topology graphs
	vector<Contact>::iterator ic1 = contacts->begin();
	vector<Contact>::iterator ic2 = contacts->end();
	for (; ic1 != ic2; ++ic1)
	{
		Contact contact = *ic1;
		double contactStart = contact.getStart();
		double contactEnd = contact.getEnd();
		int contactSource = contact.getSourceEid();
		int contactDestination = contact.getDestinationEid();
		int contactId = contact.getId();


		for (size_t ii1 = 0; ii1<stateTimes.size()-1; ++ii1)
		{
			double stateStart = stateTimes.at(ii1);
			double stateEnd = 0.0;
			++ii1;
			stateEnd = stateTimes.at(ii1);
			--ii1;

			if (stateStart >= contactStart && stateStart < contactEnd)
			{
				TopologyGraph topGraph = topology[stateStart];

				// find vertices corresponding to source and destination contact nodes
				int foundVertices = 0;
				TopologyGraph::vertex_iterator vi1, vi2;
				tie(vi1, vi2) = vertices(topGraph);
				TopologyGraph::vertex_descriptor vertexSource;
				TopologyGraph::vertex_descriptor vertexDestination;
				for (; vi1 != vi2; ++vi1)
				{
					TopologyGraph::vertex_descriptor vertex = *vi1;
					if (topGraph[vertex].eid == contactSource)
					{
						vertexSource = vertex;
						++foundVertices;
					}
					else if (topGraph[vertex].eid == contactDestination)
					{
						vertexDestination = vertex;
						++foundVertices;
					}
					if (foundVertices == 2)
					{
						break;
					}
				}

				// adding edge to found vertices
				TopologyGraph::edge_descriptor edge = (add_edge(vertexSource, vertexDestination, topGraph)).first;
				topGraph[edge].id = contactId;
				double stateCapacity = (stateEnd - stateStart) * contact.getDataRate();
				topGraph[edge].stateCapacity = stateCapacity;

				topology[stateStart] = topGraph;
			}
		}
	}

	return topology;
}

void saveGraphs(map<double, TopologyGraph> *topology, std::string outFileLocation)
{
	TopologyGraph initialGraph = (topology->begin())->second;
	int verticesNumber = num_vertices(initialGraph);

	map<double, TopologyGraph>::iterator it1 = topology->begin();
	map<double, TopologyGraph>::iterator it2 = topology->end();

	ofstream ofs(outFileLocation.c_str());

	ofs << "digraph G { \n\n";
	string edgeString = " -> ";

	ofs << "rank=same; " << endl;
	ofs << "ranksep=equally; " << endl;
	ofs << "nodesep=equally; " << endl;
	ofs << "\n\n";

	int k = 1;
	for (; it1 != it2; ++it1)
	{
		double state = it1->first;
		TopologyGraph graph = it1->second;

		ofs << "// k = " << k << ", state start = " << state << "s" << endl;

		// write all vertices labels
		typename TopologyGraph::vertex_iterator vi, vi_end, vi2, vi2_end;
		for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
		{
			int vertexId = graph[*vi].eid;
			ofs << vertexId << "." << state << " [label=L" << vertexId << "];" << endl;
		}

		// write last dummy vertex that will contain state data
		int vertexId = verticesNumber + 1;
		string stateStart = to_string((int) graph[graph_bundle].stateStart);
		string stateEnd = to_string((int) graph[graph_bundle].stateEnd);
		string st = string("\\n") + "start: " + stateStart + string("\\n") + "end: " + stateEnd + string("\\n");
		string labelString = string("\"") + string("k: ") + to_string(k++) + st + string("\"");
		ofs << vertexId << "." << state << " [shape=box,fontsize=16,label=" << labelString << "];" << endl;

		// write invisible edges between consecutive edges
		// in order to get a nicer graph disposition
		for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
		{
			for (tie(vi2, vi2_end) = vertices(graph); vi2 != vi2_end; ++vi2)
			{
				if (vi2 != vi && *vi2 > *vi)
				{
					int v1Id = graph[*vi].eid;
					int v2Id = graph[*vi2].eid;
					ofs << v1Id << "." << state << edgeString << v2Id << "." << state << " [style=\"invis\"];" << endl;
					break;
				}
			}
		}
		int lastVertexId = verticesNumber;
		ofs << lastVertexId << "." << state << edgeString << vertexId << "." << state << " [style=\"invis\"];" << endl;

		// write real topology edges
		TopologyGraph::edge_iterator ei, ei_end;
		for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
		{
			int v1Id = graph[source(*ei, graph)].eid;
			int v2Id = graph[target(*ei, graph)].eid;
			string weight = string("id:") + to_string(graph[*ei].id) + string("\\n") + to_string((int) graph[*ei].stateCapacity);
			ofs << v1Id << "." << state << edgeString << v2Id << "." << state << " [color=black,fontcolor=black,label=\"" << weight << "\",penwidth=2];" << endl;
		}

		ofs << "\n";
	}

	ofs << "// Ranks" << endl;

	// write ranks to same nodes in different states
	// in order to get a nicer graph distribution
	TopologyGraph graph = (topology->begin())->second;
	TopologyGraph::vertex_iterator vi, vi_end;
	tie(vi, vi_end) = vertices(graph);
	while (true)
	{
		ofs << "{ rank = same; ";

		int vertexId = 0;
		if (vi != vi_end)
		{
			vertexId = graph[*vi].eid;

		}
		else
		{
			vertexId = verticesNumber + 1;
		}

		map<double, TopologyGraph>::iterator iit1 = topology->begin();
		map<double, TopologyGraph>::iterator iit2 = topology->end();
		for (; iit1 != iit2; ++iit1)
		{
			ofs << vertexId << "." << iit1->first << "; ";
		}

		ofs << "}\n";

		if(vi == vi_end)
		{
			break;
		}

		++vi;
	}

	ofs << "\n\n";
	ofs << "}" << endl;
	ofs.close();

	//create pdf from .dot
	string command = "dot -Tpdf " + outFileLocation + " -o " + outFileLocation + string(".pdf");
	system(command.c_str());
}

void printGraph(TopologyGraph topologyGraph)
{
	TopologyGraph::edge_iterator ei, ei_end;
	for (tie(ei, ei_end) = edges(topologyGraph); ei != ei_end; ++ei)
	{
		int v1Id = topologyGraph[source(*ei, topologyGraph)].eid;
		int v2Id = topologyGraph[target(*ei, topologyGraph)].eid;

		cout << "edge = " << topologyGraph[*ei].id << " between nodes " << v1Id << " and " << v2Id << endl;
	}
}

}

#endif /* USE_BOOST_LIBRARIES */

