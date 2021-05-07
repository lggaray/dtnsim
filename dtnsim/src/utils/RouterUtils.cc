#include "RouterUtils.h"

#ifdef USE_BOOST_LIBRARIES

using namespace std;
using namespace boost;

namespace routerUtils
{

map<double, RouterGraph> computeFlows(ContactPlan *contactPlan, int nodesNumber, string bundleMapsLocation)
{
	map<double, RouterGraph> flows;

	// compute topology
	map<double, TopologyGraph> topology = topologyUtils::computeTopology(contactPlan, nodesNumber);

	// create empty flows as a copy of topology
	map<double, TopologyGraph>::iterator it1 = topology.begin();
	map<double, TopologyGraph>::iterator it2 = topology.end();
	for (; it1 != it2; ++it1)
	{
		// create routeGraph and add vertices
		RouterGraph routerGraph;
		for (int i = 1; i <= nodesNumber; i++)
		{
			RouterGraph::vertex_descriptor vertex = add_vertex(routerGraph);
			routerGraph[vertex].eid = i;
			routerGraph[vertex].bufferCapacity = numeric_limits<double>::max();
		}

		double stateTime = it1->first;
		TopologyGraph topologyGraph = it1->second;

		routerGraph[graph_bundle].stateStart = topologyGraph[graph_bundle].stateStart;
		routerGraph[graph_bundle].stateEnd = topologyGraph[graph_bundle].stateEnd;

		// fill routerGraph starting from topologyGraph
		TopologyGraph::edge_iterator ei, ei_end;
		for (tie(ei, ei_end) = edges(topologyGraph); ei != ei_end; ++ei)
		{
			int edgeId = topologyGraph[*ei].id;
			int v1Id = topologyGraph[source(*ei, topologyGraph)].eid;
			int v2Id = topologyGraph[target(*ei, topologyGraph)].eid;

			// find vertices corresponding to v1Id and v2Id in RouterGraph and save them
			// in vertexSource and vertexDestination
			int foundVertices = 0;
			RouterGraph::vertex_iterator vi1, vi2;
			tie(vi1, vi2) = vertices(routerGraph);
			RouterGraph::vertex_descriptor vertexSource;
			RouterGraph::vertex_descriptor vertexDestination;
			for (; vi1 != vi2; ++vi1)
			{
				RouterGraph::vertex_descriptor vertex = *vi1;
				if (routerGraph[vertex].eid == v1Id)
				{
					vertexSource = vertex;
					++foundVertices;
				}
				else if (routerGraph[vertex].eid == v2Id)
				{
					vertexDestination = vertex;
					++foundVertices;
				}
				if (foundVertices == 2)
				{
					break;
				}
			}

			// add edge between the found vertices in RouterGraph
			RouterGraph::edge_descriptor edge = (add_edge(vertexSource, vertexDestination, routerGraph)).first;
			routerGraph[edge].id = edgeId;
			routerGraph[edge].stateCapacity = topologyGraph[*ei].stateCapacity;
		}

		flows[stateTime] = routerGraph;
	}

	// add flows
	for (int i = 1; i <= nodesNumber; i++)
	{
		string fileName = bundleMapsLocation + "/BundleMap_Node" + to_string(i) + ".csv";

		ifstream file;
		file.open(fileName.c_str());
		if (!file.is_open())
		{
			continue;
			//throw cException(("Error: wrong path to contacts file " + string(fileName)).c_str());
		}

		// read first line
		string str1;
		string str2;
		getline(file, str1, '\n');

		double simTime;
		int src, dst, tsrc, tdst;
		double bitLength;
		double durationSecs;

		// read other lines
		while (!file.eof())
		{
			getline(file, str1, '\n');

			size_t posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			if (str2.empty())
			{
				break;
			}
			simTime = stod(str2);

			str1 = str1.substr(posComma + 1, string::npos);
			posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			src = stoi(str2);

			str1 = str1.substr(posComma + 1, string::npos);
			posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			dst = stoi(str2);

			str1 = str1.substr(posComma + 1, string::npos);
			posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			tsrc = stoi(str2);

			str1 = str1.substr(posComma + 1, string::npos);
			posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			tdst = stoi(str2);

			str1 = str1.substr(posComma + 1, string::npos);
			posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			bitLength = stod(str2);

			str1 = str1.substr(posComma + 1, string::npos);
			posComma = str1.find(",");
			str2 = str1.substr(0, posComma);
			durationSecs = stod(str2);

			// get state
			double state = 0;
			map<double, TopologyGraph>::iterator iit1 = topology.begin();
			map<double, TopologyGraph>::iterator iit2 = topology.end();
			for (; iit1 != iit2; ++iit1)
			{
				double stateStart = iit1->first;
				double stateEnd = std::numeric_limits<double>::max();

				++iit1;
				if (iit1 != iit2)
				{
					stateEnd = iit1->first;
				}
				--iit1;

				if (simTime >= stateStart && simTime < stateEnd)
				{
					state = stateStart;
				}
			}

			// get Router graph in state
			RouterGraph rGraph = flows[state];

			// find vertices corresponding to tsrc and tdst in RouterGraph and save them
			// in vertexSourceAux and vertexDestinationAux
			int foundVerticesAux = 0;
			RouterGraph::vertex_iterator vii1, vii2;
			tie(vii1, vii2) = vertices(rGraph);
			RouterGraph::vertex_descriptor vertexSourceAux;
			RouterGraph::vertex_descriptor vertexDestinationAux;
			for (; vii1 != vii2; ++vii1)
			{
				RouterGraph::vertex_descriptor vertex = *vii1;
				if (rGraph[vertex].eid == src)
				{
					vertexSourceAux = vertex;
					++foundVerticesAux;
				}
				else if (rGraph[vertex].eid == dst)
				{
					vertexDestinationAux = vertex;
					++foundVerticesAux;
				}
				if (foundVerticesAux == 2)
				{
					break;
				}
			}

			// find corresponding edge and add flow into flows structure
			pair<RouterGraph::edge_descriptor, int> edgePair = boost::edge(vertexSourceAux, vertexDestinationAux, rGraph);
			if (!edgePair.second)
			{
				cout << "edge not found !" << endl;
				exit(1);
			}
			else
			{
				//cout<<"edge found"<<endl;
				RouterGraph::edge_descriptor ed = edgePair.first;
				rGraph[ed].flows[tsrc][tdst] += ((double) bitLength / (double) 8);
			}

			flows[state] = rGraph;
		}
		file.close();
	}

	return flows;
}

void saveGraphs(ContactPlan *contactPlan, map<double, RouterGraph> *flows, vector<string> dotColors, map<pair<int, int>, unsigned int> flowIds, std::string outFileLocation)
{
	RouterGraph initialGraph = (flows->begin())->second;
	int verticesNumber = num_vertices(initialGraph);

	map<double, RouterGraph>::iterator it1 = flows->begin();
	map<double, RouterGraph>::iterator it2 = flows->end();

	ofstream ofs(outFileLocation.c_str());

	ofs << "digraph G { \n\n";
	string edgeString = " -> ";

	ofs << "rank=same; " << endl;
	ofs << "ranksep=equally; " << endl;
	ofs << "nodesep=equally; " << endl;
	ofs << "\n\n";

	//string totalTxBytes = "TotalTxBytes: " + to_string((int) routerUtils::getTotalTxBytes(contactPlan, flows)) + " bytes";
	//string maxDeliveryTime = "MaxDeliveryTime: " + to_string((int) routerUtils::getMaxDeliveryTime(contactPlan, flows)) + " s";

	//ofs << "label= \"\\n\\nDotted Lines = Contacts \\n" << "Coloured Lines = Traffic Flows \\n\\n" << totalTxBytes << "\\n" << maxDeliveryTime << "\"";
	ofs << "label= \"\\n\\nDotted Lines = Contacts \\n" << "Coloured Lines = Traffic Flows " << "\"";
	ofs << "\n\n";

	int k = 1;
	for (; it1 != it2; ++it1)
	{
		double state = it1->first;
		RouterGraph graph = (it1->second);

		ofs << "// k = " << k << ", state start = " << state << "s" << endl;

		// write all vertices labels
		typename RouterGraph::vertex_iterator vi, vi_end, vi2, vi2_end;
		for (tie(vi, vi_end) = vertices(graph); vi != vi_end; ++vi)
		{
			int vertexId = graph[*vi].eid;
			ofs << vertexId << "." << state << " [label=L" << vertexId << "];" << endl;
		}

		// write last dummy vertex that will contain state data
		int vertexId = verticesNumber + 1;
		int stateInt = (int) state;
		string stateStart = to_string((int) graph[graph_bundle].stateStart);
		string stateEnd = to_string((int) graph[graph_bundle].stateEnd);
		string st = string("\\n") + "start: " + stateStart + string("\\n") + "end: " + stateEnd + string("\\n");
		string labelString = string("\"") + string("k: ") + to_string(k++) + st + string("\"");
		ofs << vertexId << "." << stateInt << "[shape=box,fontsize=16,label=" << labelString << "];" << endl;

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
					ofs << v1Id << "." << stateInt << edgeString << v2Id << "." << stateInt << "[style=\"invis\"];" << endl;
					break;
				}
			}
		}
		int lastVertexId = verticesNumber;
		ofs << lastVertexId << "." << stateInt << edgeString << vertexId << "." << stateInt << "[style=\"invis\"];" << endl;

		// write real topology and flow edges
		RouterGraph::edge_iterator ei, ei_end;
		for (tie(ei, ei_end) = edges(graph); ei != ei_end; ++ei)
		{
			// topology edges

			int v1Id = graph[source(*ei, graph)].eid;
			int v2Id = graph[target(*ei, graph)].eid;
			string weight = string("id:") + to_string(graph[*ei].id) + string("\\n") + to_string((int) graph[*ei].stateCapacity);
			ofs << v1Id << "." << stateInt << edgeString << v2Id << "." << stateInt << "[color=grey,fontcolor=grey, style=\"dotted\", label=\"" << weight << "\",penwidth=2];" << endl;

			// flow edges
			for (int k1 = 1; k1 <= verticesNumber; k1++)
			{
				for (int k2 = 1; k2 <= verticesNumber; k2++)
				{
					if (graph[*ei].flows[k1][k2] != 0)
					{
						string flowValue = to_string((int) graph[*ei].flows[k1][k2]);
						string edgeColor = dotColors.at(flowIds[make_pair(k1, k2)]);
						string flowId = to_string(k1) + "-" + to_string(k2);
						string edgeDescription = to_string(v1Id) + "." + to_string(stateInt) + edgeString + to_string(v2Id) + "." + to_string(stateInt);
						string edgeProperties = string("[fontsize=15, penwidth=2, color=") + string("\"") + edgeColor + string("\"");
						edgeProperties.append(string(",label=") + string("\"") + flowId + string("\\n") + flowValue + string("\"") + string("]"));
						ofs << edgeDescription + edgeProperties << endl;
					}
				}
			}

			ofs << "\n";
		}
	}

	ofs << "\n";
	ofs << "// Ranks" << endl;

	// write ranks to same nodes in different states
	// in order to get a nicer graph distribution
	RouterGraph graphAux = (flows->begin())->second;
	RouterGraph::vertex_iterator vii, vii_end;
	tie(vii, vii_end) = vertices(graphAux);
	while (true)
	{
		ofs << "{ rank = same; ";

		int vertexId = 0;
		if (vii != vii_end)
		{
			vertexId = graphAux[*vii].eid;

		}
		else
		{
			vertexId = verticesNumber + 1;
		}

		map<double, RouterGraph>::iterator iit1 = flows->begin();
		map<double, RouterGraph>::iterator iit2 = flows->end();
		for (; iit1 != iit2; ++iit1)
		{
			ofs << vertexId << "." << iit1->first << "; ";
		}

		ofs << "}\n";

		if (vii == vii_end)
		{
			break;
		}

		++vii;
	}

	ofs << "\n\n";
	ofs << "}" << endl;
	ofs.close();

	//create pdf from .dot
	string command = "dot -Tpdf " + outFileLocation + " -o " + outFileLocation + string(".pdf");
	system(command.c_str());
}

void printGraph(RouterGraph routerGraph)
{
	int verticesNumber = num_vertices(routerGraph);

	RouterGraph::edge_iterator ei, ei_end;
	for (tie(ei, ei_end) = edges(routerGraph); ei != ei_end; ++ei)
	{
		int v1Id = routerGraph[source(*ei, routerGraph)].eid;
		int v2Id = routerGraph[target(*ei, routerGraph)].eid;

		cout << "edge = " << routerGraph[*ei].id << " between nodes " << v1Id << " and " << v2Id << endl;

		cout << "flows = " << endl;
		typename RouterGraph::vertex_iterator vi, vi_end, vi2, vi2_end;
		for (int k1 = 0; k1 <= verticesNumber; k1++)
		{
			for (int k2 = 0; k2 <= verticesNumber; k2++)
			{
				cout << routerGraph[*ei].flows[k1][k2] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
}

vector<string> getDotColors()
{
	vector < string > dotColors(20);
	dotColors.at(0) = "darkgreen";
	dotColors.at(1) = "chartreuse2";
	dotColors.at(2) = "darkgoldenrod1";
	dotColors.at(3) = "crimson";
	dotColors.at(4) = "navyblue";
	dotColors.at(5) = "burlywood3";
	dotColors.at(6) = "brown1";
	dotColors.at(7) = "darkorchid3";
	dotColors.at(8) = "darkslategrey";
	dotColors.at(9) = "dodgerblue";
	dotColors.at(10) = "red";
	dotColors.at(11) = "purple3";
	dotColors.at(12) = "yellow2";
	dotColors.at(13) = "turquoise2";
	dotColors.at(14) = "seagreen3";
	dotColors.at(15) = "orange";
	dotColors.at(16) = "thistle3";
	dotColors.at(17) = "indigo";
	dotColors.at(18) = "gray9";
	dotColors.at(19) = "palegreen4";

	return dotColors;
}

map<int, double> getUsedContacts(ContactPlan *contactPlan, map<double, RouterGraph> *flows)
{
	map < int, double>  contactsTraffic;

	vector<Contact> contacts = *(contactPlan->getContacts());

	for(size_t i = 0; i<contacts.size(); i++)
	{
		Contact contact = contacts.at(i);
		int contactId = contact.getId();

		map<double, RouterGraph>::iterator it1 = flows->begin();
		map<double, RouterGraph>::iterator it2 = flows->end();

		for(; it1 != it2; ++it1)
		{
			RouterGraph routerGraph = it1->second;
			int verticesNumber = num_vertices(routerGraph);

			RouterGraph::edge_iterator ei, ei_end;
			for (tie(ei, ei_end) = edges(routerGraph); ei != ei_end; ++ei)
			{
				if(routerGraph[*ei].id == contactId)
				{
					for (int k1 = 1; k1 <= verticesNumber; k1++)
					{
						for (int k2 = 1; k2 <= verticesNumber; k2++)
						{
							if (routerGraph[*ei].flows[k1][k2] != 0)
							{
								map < int, double>::iterator ii = contactsTraffic.find(contactId);
								if(ii != contactsTraffic.end())
								{
									contactsTraffic[contactId] += routerGraph[*ei].flows[k1][k2];
								}
								else
								{
									contactsTraffic[contactId] = routerGraph[*ei].flows[k1][k2];
								}
							}
						}
					}
				}
			}
		}
	}

	return contactsTraffic;
}

double getMaxDeliveryTime(ContactPlan *contactPlan, map<double, RouterGraph> *flows)
{
	double maxDeliveryTime = 0.0;

	map<int, double> usedContacts = getUsedContacts(contactPlan, flows);

	map<int, double>::iterator it1 = usedContacts.begin();
	map<int, double>::iterator it2 = usedContacts.end();
	for(; it1 != it2; ++it1)
	{
		int contactId = it1->first;
		Contact contact = *(contactPlan->getContactById(contactId));

		double start = contact.getStart();
		double byteRate = contact.getDataRate();
		double txDuration = it1->second / (byteRate);

		double contactEndUsed = start + txDuration;

		if(contactEndUsed > maxDeliveryTime)
		{
			maxDeliveryTime = contactEndUsed;
		}
	}

	return maxDeliveryTime;
}

double getTotalTxBytes(ContactPlan *contactPlan, map<double, RouterGraph> *flows)
{
	double totalTxBytes = 0.0;

	map<int, double> usedContacts = getUsedContacts(contactPlan, flows);

	map<int, double>::iterator it1 = usedContacts.begin();
	map<int, double>::iterator it2 = usedContacts.end();
	for(; it1 != it2; ++it1)
	{
		totalTxBytes += it1->second;
	}

	return totalTxBytes;
}

}

#endif /* USE_BOOST_LIBRARIES */

