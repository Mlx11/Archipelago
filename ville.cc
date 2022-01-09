
// Architecture: Fig 11 b1

// file: ville.cc
// author: Marc Bachelet, Michael Linder
// last edited: 24.02.2020
// status: tested
// to-do: 


#include "error.h"
#include "city_gui.h"
#include "constantes.h"

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

static City city;
static unsigned int selected_node_uid{no_link};

// Environment type (file or gui) 
static Environment env(gui);

#define BENCHMARK

/* 			
				C  I  T  Y 
*/

//------------ MODIFYING CITY -------------------------------------------------

void City::reset()
{
	nodes_by_uid.clear();
	connections_by_id.clear();
	outgoing_connections_id_by_uid.clear();
	next_uid = 0;
	next_cid = 0;
	criteria.reset();
	recalculate_criteria();
}


void City::add_node(Node& n)
{
	if(uid_already_exists(n.get_uid()))
	{
		// If same uid in file, throw error
		if(env == file)
		{
			throw (string(error::identical_uid(n.get_uid())));
		}
		
		// Else increment uid until it is valid.
		while(uid_already_exists(n.get_uid()))
		{
			n.set_uid(get_next_uid());
		}
	}

	test_node_fits_in(n);

	// If no erros thrown, insert in data structure.
	nodes_by_uid.insert(pair<unsigned int, Node>(n.get_uid(), n));
	vector<unsigned int> vect{};
	outgoing_connections_id_by_uid.insert(pair<unsigned int,
									 vector<unsigned int>> (n.get_uid(), vect));

	// Adding contribution for criteria
	criteria+= n;
}


void City::del_node(unsigned int id)
{
	// Deleting node for criteria
	criteria -= (nodes_by_uid.find(id)->second);

	// Erasing all outgoing connections
	vector<unsigned int> vect_outgoing = outgoing_connections_id_by_uid.
														find(id)->second;
	for(auto c_id: vect_outgoing)
	{
		city.del_connection(c_id);
	}

	// Erasing uid in outgoing_connections_by_uid
	outgoing_connections_id_by_uid.erase(id);

	// Erasing node itself
	nodes_by_uid.erase(id);
}


void City::add_connection(unsigned int uid_A, unsigned int uid_B)
{
	Node A = nodes_by_uid.find(uid_A)->second;
	Node B = nodes_by_uid.find(uid_B)->second;
	
	Connection c(A, B);
	
	// Testing for errors:
	test_multiple_same_link(c);
	
	if(uid_A == uid_B)
	{
		throw string(error::self_link_node(uid_A));
	}
	
	test_link_vacuum(uid_A);
	test_link_vacuum(uid_B);
		
	test_link_limit(uid_A);
	test_link_limit(uid_B);

	test_connection_fits_in(c);
	
	// If no errors thrown, inserting in data structure:
	unsigned int c_id{get_next_cid()};
	outgoing_connections_id_by_uid.find(uid_A)->second.push_back(c_id);
	outgoing_connections_id_by_uid.find(uid_B)->second.push_back(c_id);

	// Add in connections_by_id
	connections_by_id.insert(pair<unsigned int, Connection>(c_id, c));

	// Update CI:
	criteria += c;
}


void City::del_connection(unsigned int c_id)
{	
	Connection c = connections_by_id.find(c_id)-> second;
	
	// Remove criteria contribution
	criteria -= c;

	// Deleting connection in outgoing connections of node A

	if(outgoing_connections_id_by_uid.find(c.get_uid_of_point_A())->second.size() != 0)
	{
		vector<unsigned int>& vecA(outgoing_connections_id_by_uid.
								   find(c.get_uid_of_point_A())->second);
		vecA.erase(find(vecA.begin(), vecA.end(), c_id));
	} 

	// Deleting connection in outgoing connections of node B:
	if(outgoing_connections_id_by_uid.find(c.get_uid_of_point_B())->second.size() != 0)
	{
		vector<unsigned int>& vecB(outgoing_connections_id_by_uid.
								   find(c.get_uid_of_point_B())->second);
		vecB.erase(find(vecB.begin(), vecB.end(), c_id));
	} 

	// Erasing connection itself
	connections_by_id.erase(c_id);
}


void City::del_connection(unsigned int uid_A, unsigned int uid_B)
{
	if(outgoing_connections_id_by_uid.count(uid_A))
	{
		for(const unsigned int c_id : outgoing_connections_id_by_uid.at(uid_A))
		{
			Connection con = (connections_by_id.find(c_id)->second);
			// and there is a link going to uid_B
			if((con.get_uid_of_point_A() == uid_B)
				or (con.get_uid_of_point_B() == uid_B))
			{
				del_connection(c_id);
				return;
			}
		}
	}
}


void City::update_connections_to_selected_node()
{
	vector<Connection> new_connections;

	// create all the new connections
	for(unsigned int c_id: outgoing_connections_id_by_uid[selected_node_uid])
	{
		Connection con = connections_by_id.find(c_id)->second;
		Connection new_connection(nodes_by_uid.find(con.get_uid_of_point_A())->second,
								  nodes_by_uid.find(con.get_uid_of_point_B())->second);
		test_connection_fits_in(new_connection);
		new_connections.push_back(new_connection);
	}

	// replace old with new connections
	int i{0};
	for(unsigned int c_id: outgoing_connections_id_by_uid[selected_node_uid])
	{
		connections_by_id.find(c_id)->second = new_connections[i];
		i++;
	}
}


void City::move_selected_node_to_coord(Coord new_position)
{
	Node& node = nodes_by_uid.find(selected_node_uid)->second;

	// Remove previous node criteria contribution
	criteria -= node;
	// Remove criteria contribution of its outgoing connections
	vector<unsigned int> vect_outgoing(city.outgoing_connections_id_by_uid.
									   find(node.get_uid())->second);
	for(auto c_id: vect_outgoing)
	{
		criteria -= city.connections_by_id.find(c_id)->second;
	}

	// Move nod
	node.set_position(new_position);
	test_node_fits_in(node);
	//~ nodes_by_uid.find(selected_node_uid)->second = node;
	update_connections_to_selected_node();

	// Add contributions to criteria (old or new)
	criteria += node;
	vect_outgoing = city.outgoing_connections_id_by_uid.find(node.get_uid())->second;
	for(auto c_id: vect_outgoing)
	{
		criteria += city.connections_by_id.find(c_id)->second;
	}
}


void City::increase_radius_of_node(unsigned int uid, double factor)
{
	Node& n = nodes_by_uid.find(uid)->second;
	unsigned int pre_population = n.get_population();
	double new_population = pow(sqrt(pre_population)+factor, 2);

	// Test if new population is possible
	if(new_population > max_capacity){new_population = max_capacity;}
	if(new_population < min_capacity){new_population = min_capacity;}
	
	// Deleting previous node for criteria (with old population)
	criteria -= n;

	n.set_population(new_population);

	// Test if node fits in
	try
	{
		test_node_fits_in(n);
	} 
	catch(string err) 
	{
		n.set_population(pre_population);
		throw err;
	}

	// Adding node again for criteria (with or without changes)
	criteria+= n;

	recalculate_criteria();
}


// ------------- GETTERS ------------------------------------------


unsigned int City::get_smallest_population(const Connection &c)
{
	unsigned int uid_A = c.get_uid_of_point_A();
	unsigned int uid_B = c.get_uid_of_point_B();

	// Finding node population and returning smallest
	return min(nodes_by_uid.find(uid_A)->second.get_population(),
			   nodes_by_uid.find(uid_B)->second.get_population());

}


unsigned int City::get_node_uid_near_coords(Coord c)
{
	Circle circle_click(c, 0);
	for(auto& element: city.nodes_by_uid)
	{
		// Return node uid if click was in its circle.
		if(element.second.get_circle().dist_to_circle(circle_click) <= 0)
		{
			return element.first;
		}
	}

	// If click is not in a circle
	return no_link;
}


Coord City::get_coords_of_node(unsigned int uid)
{
	return nodes_by_uid.find(uid)->second.get_circle().get_center_coordinates();
}

//-----------------CRITERIA----------------------------------------------------

void City::recalculate_criteria()
{
	city.criteria.calculate_enj();
	city.criteria.calculate_mta();	
}


Criteria_values City::get_criteria()
{
	return criteria.get_criteria();
}


//-----------------DRAWING ----------------------------------------------------


void City::on_draw(bool show_shortest_path)
{
	// Update shortest path
	dijkstra.invalidate_shortest_path();
	dijkstra.run_algorithm(selected_node_uid);

	// Draw city
	city.draw_connections(show_shortest_path);
	city.draw_nodes(show_shortest_path);
}


void City::draw_nodes(bool show_shortest_path)
{
	map<unsigned int, Color> nodes_with_color;

	// Default color black
	for(auto& element: city.nodes_by_uid)
	{
		nodes_with_color.insert(pair<unsigned int, Color>(element.first, BLACK));
	}

	// Shortest path nodes set to green
	if(show_shortest_path)
	{
		for(unsigned int id : dijkstra.get_shortest_path_nodes())
		{
			nodes_with_color.find(id)->second = GREEN;
		}
	}

	// Selected node set to red
	nodes_with_color.find(selected_node_uid)->second = RED;

	// Draw
	for(auto &el: nodes_with_color)
	{
		nodes_by_uid.find(el.first)->second.on_draw(el.second);
	}
}


void City::draw_connections(bool show_shortest_path)
{
	map<unsigned int, Color> connections_with_color;
	
	// Default color black.
	for(auto& element: city.connections_by_id)
	{
		connections_with_color.insert(pair<unsigned int, Color>(element.first, BLACK));
	}
	
	// Shortest path connections set to green
	if(show_shortest_path)
	{
		for(unsigned int id : dijkstra.get_shortest_path_connections())
		{
			connections_with_color.find(id)->second = GREEN;
		}
	}
	
	// Draw
	for(auto &el: connections_with_color)
	{
		connections_by_id.find(el.first)->second.on_draw(el.second);
	}
}



//-----------------TEST FUNCTIONS-----------------------------------------


void City::test_node_fits_in(const Node& n) const
{
	switch(env)
	{
		case gui: 
		test_distances(n, dist_min);

		case file:
		test_distances(n, 0);
	}
}


void City::test_distances(const Node&n, const double minimal_distance) const
{
	// Test for error: node_node_superposition
	for(auto& element : nodes_by_uid)
	{
		if(n.distance_to_node(element.second) <= minimal_distance
		   && n.get_uid() != element.second.get_uid())
		{
			throw string(error::node_node_superposition(n.get_uid(),
						 element.second.get_uid()));
		}
	} 
		
	// Test for error: node_link_superposition
	for(auto& element : connections_by_id)
	{
		if(n.distance_to_connection(element.second) <= minimal_distance)
		{
			if(not (element.second.get_uid_of_point_A() == n.get_uid()
			   || element.second.get_uid_of_point_B() == n.get_uid()))
			{
				throw string(error::node_link_superposition(n.get_uid()));
			}
		}
	}
}


void City::test_connection_fits_in(const Connection& c) const
{
	switch(env)
	{
		case gui: 
		test_distances(c, dist_min);

		case file:
		test_distances(c, 0);
	}
}


void City::test_distances(const Connection& c, const double minimal_distance) const
{
	for(auto& el : nodes_by_uid)
	{
		bool superposition{el.second.distance_to_connection(c) < minimal_distance};
		bool node_not_linked_to_connection{not c.is_connected_to(el.second.get_uid())};
		
		// Both nodes the connection is linked to are not producing an error.
		if(superposition and node_not_linked_to_connection)
		{			
			throw string(error::node_link_superposition(el.second.get_uid()));
		}
	}
}


void City::test_link_vacuum(unsigned int uid) const
{
	if((nodes_by_uid.find(uid)==nodes_by_uid.end()))
	{
		throw string(error::link_vacuum(uid)) ;
	}
}


void City::test_link_limit(unsigned int uid) const
{
	// Finding number of outgoing connections:
	unsigned int link((outgoing_connections_id_by_uid.find(uid)->second).size());
	unsigned int link_limit = (nodes_by_uid.find(uid)->second).get_link_limit();

	if (link >= link_limit)
	{
		throw string(error::max_link(uid)) ;	
	}	
}


void City::test_multiple_same_link(const Connection& c) const
{
	unsigned int uid_A{c.get_uid_of_point_A()};
	unsigned int uid_B{c.get_uid_of_point_B()};
	
	// If uid_A contained in uid->connections map
	if(outgoing_connections_id_by_uid.count(uid_A))
	{
		for(const unsigned int c_id : outgoing_connections_id_by_uid.at(uid_A))
		{
			Connection con = (connections_by_id.find(c_id)->second);
			
			// and there is a link going to uid_B
			if((con.get_uid_of_point_A() == uid_B)
				or (con.get_uid_of_point_B() == uid_B))
			{
				throw(string(error::multiple_same_link(uid_A, uid_B)));
			}
		}
	}
}


//--------------UID FUNCTIONS----------------------------------------------


unsigned int City::get_next_uid()
{
	return ++next_uid;
}


unsigned int City::get_next_cid()
{
	return ++next_cid;
}


bool City::uid_already_exists(unsigned int id) const
{
	return not (nodes_by_uid.find(id) == nodes_by_uid.end());
}

/*
			C I T Y   N E S T E D   C L A S S E S 
*/

//-------------- CITY::CRITERIA --------------------------------------------------


void City::Criteria::reset()
{
	housing_population = 0;
	other_population = 0;
	criteria_values = {0., 0., 0.};
}


Criteria_values City::Criteria::get_criteria()
{
	return criteria_values;
}


void City::Criteria::operator+=(const Node &n)
{
	node_contribution(n , 1);
}


void City::Criteria::operator-=(const Node &n)
{
	node_contribution(n , -1);
}


void City::Criteria::operator+=(const Connection &c)
{
	connection_contribution(c , 1);
}

void City::Criteria::operator-=(const Connection &c)
{
	connection_contribution(c, -1);
}


void City::Criteria::calculate_enj()
{
	if((housing_population + other_population)==0) // preventing zero division
	{
		criteria_values.enj = 0;
		return;
	}
	
	criteria_values.enj = (housing_population  * 1.0 - other_population) /
						  (housing_population + other_population);
}


void City::Criteria::calculate_mta()
{
	bool time_only = true;
	unsigned int total_number_housing_nodes{0};

	criteria_values.mta = 0.;

	// Summing access time for all housing nodes
	for(auto& element : city.nodes_by_uid)
	{
		if(element.second.get_type() == HOUSING)
		{
			total_number_housing_nodes += 1;
			pair<double, double> fastest_access;
			fastest_access = city.dijkstra.run_algorithm(element.second.get_uid(),
														 time_only);
		
			criteria_values.mta+= fastest_access.first + fastest_access.second;
		}
	}

	// normalize for the number of housing nodes
	if(total_number_housing_nodes != 0)
	{
		criteria_values.mta /= total_number_housing_nodes;
	}
}


void City::Criteria::node_contribution(const Node &n, const int sign)
{
	// sign allow to add or subtract the contribution in one functin
	// therefore sign must be 1 or -1
	if(pow(sign, 2) != 1)
	{
		throw "False argument in node contribution";
	}
	
	// Add criteria contribution of node itself
	if(n.get_type()==HOUSING)
	{
		housing_population += sign * n.get_population();
	}
	else 
	{
		other_population += sign * n.get_population();
	}
}


void City::Criteria::connection_contribution(const Connection &c, 
											 const int sign) 
{
	double length(c.get_line().get_length());
	unsigned int smaller_capacity(city.get_smallest_population(c));

	// Adding connection contribution to CI
	criteria_values.ci += sign * length * c.get_speed() * smaller_capacity;
}	


//--------------CITY::DIJKSTRA------------------------------------------------


std::pair<double, double> City::Dijkstra::run_algorithm(unsigned int start_uid, 
														bool time_only)
{
	if(exit_dijkstra(start_uid))
	{
		return pair<double, double>(0.,0.);
	}	

	init_data_structure(start_uid);

	while(not time_by_uid.empty() and not(production_found and transport_found))
	{
		// Get node with the lowest access time
		pair<unsigned int, double> lowest_access_time = get_lowest_access_time();
		Node fastest_access_node = city.nodes_by_uid.find(lowest_access_time.first)
								   ->second;
		// Remove the threated node from the map
		time_by_uid.erase(lowest_access_time.first);
		// Check if a transport or production node was reached
		if(fastest_access_node.get_type() == PRODUCTION and not production_found)
		{
			production_found = true;
			fastest_production = lowest_access_time;
			continue;
		}
		if(fastest_access_node.get_type() == TRANSPORT and not transport_found)
		{
			transport_found = true;
			fastest_transport = lowest_access_time;
		}
		update_neighbors(lowest_access_time);
	}
	std::pair<double, double> pair_access_time(fastest_production.second,
												fastest_transport.second);
	if(not time_only)
	{
		fill_shortest_path_vector(start_uid);
	}
	return pair_access_time;
}


bool City::Dijkstra::exit_dijkstra(unsigned int start_uid)
{
	if(start_uid == no_link) 
	{
		return true;
	}	

	// Only running algorithm if selected housing node.
	if(city.nodes_by_uid.find(start_uid)->second.get_type() != HOUSING)
	{
		invalidate_shortest_path();
		return true;
	}
	return false;
}


void City::Dijkstra::init_data_structure(unsigned int start_uid)
{
	time_by_uid.clear();
	parent_uid_by_uid.clear();

	fastest_production= {no_link, infinite_time};
	fastest_transport = {no_link, infinite_time};

	production_found = false;
	transport_found = false;

	time_by_uid.insert(pair<unsigned int, double>(start_uid, 0));
	parent_uid_by_uid.insert(pair<unsigned int, unsigned int>(start_uid, no_link));
}


pair<unsigned int, double> City::Dijkstra::get_lowest_access_time()
{
	pair<unsigned int, double> lowest_access_time(time_by_uid.begin()->first,
												  time_by_uid.begin()->second);

	for (auto& pair: time_by_uid)
	{
		if(pair.second < lowest_access_time.second)
		{
			lowest_access_time = pair;
		}
	}
	return lowest_access_time;
}


void City::Dijkstra::update_neighbors(pair<unsigned, double>&lowest_access_time)
{
	unsigned int uid_fastest_access = lowest_access_time.first;
	double access_time = lowest_access_time.second;


	for(unsigned int connection_id: city.
									outgoing_connections_id_by_uid[uid_fastest_access])
	{
		Connection c(city.connections_by_id.find(connection_id)->second);
		unsigned int target_uid(0);
		
		// For each connection, get neighbor uid
		target_uid = (c.get_uid_of_point_B() == uid_fastest_access) ? 
				 	  c.get_uid_of_point_A() : c.get_uid_of_point_B();
		
		// Get access time if passing through fastest access uid
		double new_access_time(access_time + c.get_travel_time());
		auto pair_target_uid(parent_uid_by_uid.find(target_uid));
		
		// If node has already been reached: compare both access times
		if(pair_target_uid != parent_uid_by_uid.end())
		{
			auto pair_uid_time(time_by_uid.find(target_uid));
			if(pair_uid_time->second > new_access_time)
			{
				pair_uid_time->second = new_access_time;
				pair_target_uid->second = uid_fastest_access;
			}
		} 
		// If node has not been reached: insert time
		else 
		{
			parent_uid_by_uid.insert(pair<unsigned, unsigned>(target_uid, 
															  uid_fastest_access));
			time_by_uid.insert(pair<unsigned, double>(target_uid, 
													  new_access_time));
		}
	}
}


void City::Dijkstra::fill_shortest_path_vector(unsigned int start_uid)
{
	shortest_path_connections.clear();
	shortest_path_nodes.clear();
	
	backtrack_path(fastest_production.first, start_uid);
	backtrack_path(fastest_transport.first, start_uid);
}


void City::Dijkstra::backtrack_path(unsigned int from, unsigned int to)
{
	// backtracking the path from -> to 
	unsigned int next_stop = 0;

	while(from != no_link)
	{
		next_stop = parent_uid_by_uid[from];

		// for each node in path, finding connection leading to parent
		for(unsigned int con_id: city.outgoing_connections_id_by_uid[next_stop])
		{
			Connection connection = city.connections_by_id.find(con_id)->second;
			
			if((connection.get_uid_of_point_A()==from) 
			or (connection.get_uid_of_point_B()==from))
			{
				shortest_path_connections.push_back(con_id);
			}
		}
		
		// Make sure a uid isnt twice in shortest_path
		if(find(shortest_path_nodes.begin(), shortest_path_nodes.end(), from) 
												== shortest_path_nodes.end()) 
		{
			shortest_path_nodes.push_back(from);
		}

		from = next_stop;
	}
}


void City::Dijkstra::invalidate_shortest_path()
{
	shortest_path_nodes.clear();
	shortest_path_connections.clear();
}


vector<unsigned int>& City::Dijkstra::get_shortest_path_nodes()
{
	return shortest_path_nodes;
}


vector<unsigned int>& City::Dijkstra::get_shortest_path_connections()
{
	return shortest_path_connections;
}


/*
			F  I  L  E     R  E  A  D  E  R
*/

void Filereader::read_file(const char * filename)
{
	read_state = 0;
	counter = 0;
	total = 0;
	env = file;

	string line;
    ifstream file(filename);
	try
	{
		if(file.fail())
		{
			throw string("error opening file");
		}
		
		while(getline(file >> ws,line)) 
		{
			if(line[0]=='#')  
			{
				continue;  
			}
			read_line(line);
		}
	}
	catch(string err)
	{
		env = gui;
		throw err;
	}

	env=gui;
	city.recalculate_criteria();
	
	throw string(error::success());
}


void Filereader::read_line(string line)
{
	istringstream data(line);

	switch(read_state) 
	{
		case NBHOUSING: read_number(data); break;
		case HOUSING_DATA: read_node<Housing>(data); break;
		case NBTRANSPORT: read_number(data); break;		
		case TRANSPORT_DATA: read_node<Transport>(data); break;
		case NBPRODUCTION: 	read_number(data); break;		
		case PRODUCTION_DATA: read_node<Production>(data); break;
		case NBLINK: read_number(data); break;
		case LINK: read_link(data); break;
		case FINISH: read_state = 0; break;		//resetting read state
	}	
}


void Filereader::read_number(istringstream& data)
{
	data >> total;
			
	counter=0 ;

	if(total==0) read_state+=2; 
	else read_state+=1; 
}


template<class T> // Node_type to be specified in function call
void Filereader::read_node(istringstream& data)
{
	unsigned int uid, population;
	double x,y;
		
	data >> uid >> x >> y >> population;
		
	counter++;
	if(counter == total)
	{
		read_state++;
	}

	Coord position = {x,y};

	// Creating new node depending on type
	T new_node(population, position, uid);
	city.add_node(new_node);
}


void Filereader::read_link(istringstream& data)
{
	unsigned int uid(0), uid_other(0);
	data >> uid >> uid_other;	

	counter++;
	if(counter == total) 
	{
		read_state++;
	}

	city.add_connection(uid, uid_other);
}


/*
			F  I  L  E    W  R  I  T  E  R
*/

string Filewriter::write()
{
	all = ""; 	 // resetting string
	
	write_all_nodes();
	write_all_connections();

	return all;
}


void Filewriter::write_all_nodes() 
{
	// Each node is added to strings depending on type.
	for(auto& element: city.nodes_by_uid)
	{
		write_node(element.second);
	}

	// Concatenating all node type informations.
	all +=  "\n # nb housing \n " + to_string(ctr_housing) + "\n # housing\n " 
			+ str_housing;
	all += "\n # nb transport \n " + to_string(ctr_transport) 
			+ "\n # transport \n" + str_transport;
 	all +=  "\n # nb production \n" + to_string(ctr_production) 
			+ "\n #  production \n" + str_production;
}

void Filewriter::write_node(const Node &n)
{
	// Get node data:
	Coord position{(n.get_circle()).get_center_coordinates()};
		
	string x = to_string(position.x);
	string y = to_string(position.y);
	replace(x.begin(), x.end(), ',', '.');
	replace(y.begin(), y.end(), ',', '.');

	string node_data{"	" + to_string(n.get_uid()) + " "
					 + x + " " + y + " "
					 + to_string(n.get_population()) + "\n"};

	// Add into corresponding string:
	switch(n.get_type())
	{
		case HOUSING: 
		{
			str_housing += node_data;	
			ctr_housing++; 
			break;   
		}
		case TRANSPORT: 
		{
			str_transport += node_data; 
			ctr_transport++;  
			break;
		}
		case PRODUCTION: 
		{
			str_production += node_data; 
			ctr_production++; 
			break;
		}
	}
}


void Filewriter::write_all_connections() 
{
	all += "\n # nb link \n" + to_string(city.connections_by_id.size()) + "\n";

	for(auto& element: city.outgoing_connections_id_by_uid)
	{		
		for(auto& es : element.second)
		{
			// For each id, find corresponding connection
			Connection c= {city.connections_by_id.find(es)->second};

			if(element.first == c.get_uid_of_point_A()) // Avoid printing twice
			{
				all+=  "	" + to_string(c.get_uid_of_point_A()) + " " 
				+ to_string(c.get_uid_of_point_B()) + "\n";
			}
		}
	}
}

/*
			C  I  T  Y    G  U  I
*/


void City_gui::reset()
{
	city.reset();
	selected_node_uid = no_link;
}


void City_gui::create_new_connection(const unsigned int target_uid)
{
	try
	{
		city.add_connection(selected_node_uid, target_uid);
	} 
	catch (string err)
	{
		if(err == error::multiple_same_link(target_uid, selected_node_uid)
		   or err == error::multiple_same_link(selected_node_uid, target_uid))
		{
			// If a multiple same link error was thrown the connection must be deleted
			City_gui::delete_connection_to_node(target_uid);
		} 
		else
		{
			// if the error wasn't handled rethrow it
			throw err;
		}
	}
	
	recalculate_criteria();
}


void City_gui::create_new_node(const Node_type type, const Coord position)
{
	Node node(min_capacity, Coord{0,0});
	switch (type)
	{
		case HOUSING: node = Housing(min_capacity, position); break;
		case PRODUCTION: node = Production(min_capacity, position); break;
		case TRANSPORT: node = Transport(min_capacity, position); break;
		default: break;
	}
	city.add_node(node);
}


void City_gui::delete_selected_node()
{
	city.del_node(selected_node_uid);
	selected_node_uid = no_link;
}


void City_gui::delete_connection_to_node(const unsigned int target_uid)
{
	city.del_connection(selected_node_uid, target_uid);
}


void City_gui::move_selected_node_to_coord(const Coord c)
{
	if(selected_node_uid == no_link)
	{
		return;
	}
	
	Coord old_coord{city.get_coords_of_node(selected_node_uid)};

	try
	{
		city.move_selected_node_to_coord(c);
	} 
	catch(string err)
	{
		// reset the position of the node in case of an error
		city.move_selected_node_to_coord(old_coord);
		throw err;
	}
	
	recalculate_criteria();
}


void City_gui::change_size_of_node(const Coord click_begin, const Coord click_end)
{
	if(selected_node_uid == no_link)
	{
		return;
	}

	Coord node_coord = city.get_coords_of_node(selected_node_uid);

	double radius_click_1{sqrt(pow(click_begin.x - node_coord.x, 2) +
							   pow(click_begin.y - node_coord.y, 2))};
	double radius_click_2{sqrt(pow(click_end.x - node_coord.x, 2) +
							   pow(click_end.y - node_coord.y, 2))};
	city.increase_radius_of_node(selected_node_uid, radius_click_2 - radius_click_1);
}

// ---------NODE SELECTION ---------------------------------------------


void City_gui::select_new_node(const unsigned int new_uid)
{
	selected_node_uid = new_uid;
}


unsigned int City_gui::get_selected_node_uid()
{
	return selected_node_uid;
}


// ---------GETTER FOR GUI----------------------------------------------


unsigned int City_gui::get_node_uid_near_coords(const Coord c)
{
	return city.get_node_uid_near_coords(c);
}

// ----------DRAWING --------------------------------------------------

void City_gui::draw_city(const bool show_shortest_path)
{
	city.on_draw(show_shortest_path);
}


//-----------CRITERIA:-------------------------------------

void City_gui::recalculate_criteria()
{
	city.recalculate_criteria();
}


Criteria_values City_gui::get_criteria()
{
	return city.get_criteria();
}
