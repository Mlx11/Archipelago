
// Architecture: Fig 11 b1

// file: ville.h
// author: Marc Bachelet, Michael Linder
// last edited: 24.03.2020
// status: tested
// to-do: -

#ifndef HEADER_GUARD_Ville_H
#define HEADER_GUARD_Ville_H

using namespace std;

#include "noeud.h"

#include <map>
#include <vector>
#include <sstream>

struct Criteria_values
{
	double mta;
	double ci;
	double enj;
};


class City
{
	friend class Filewriter;

public:		
		// MODIFYING CITY-------------------------------------------------
	// Creating/ deleting nodes and connections
	void reset();
	void add_node(Node& n);
	void del_node(unsigned int id);

	void add_connection(unsigned int uid_A, unsigned int uid_B);
	void del_connection(unsigned int c_id);
	void del_connection(unsigned int uid_A, unsigned int uid_B);

	void update_connections_to_selected_node();

	// Modifying nodes
	void move_selected_node_to_coord(Coord new_position);
	void increase_radius_of_node(unsigned int uid, double factor);

	// GETTERS-------------------------------------------------
	unsigned int get_smallest_population(const Connection &c);
	unsigned int get_node_uid_near_coords(Coord c);
	Coord get_coords_of_node(unsigned int uid);

	// CRITERIA--------------------------------------------------
	void recalculate_criteria();
	Criteria_values get_criteria();
		
	// DRAWING--------------------------------------------------
	void on_draw(bool show_shortest_path);
	
private:
	// DRAWING: --------------------------------------------------
	void draw_nodes(bool show_shortest_path);
	void draw_connections(bool show_shortest_path);

	// TEST FUNCTIONS:---------------------------------------------
	void test_node_fits_in(const Node& n) const;
	void test_distances(const Node& n, const double minimal_distance) const;

	void test_connection_fits_in(const Connection& c) const;
	void test_distances(const Connection& c, const double minimal_distance) const;
		
	void test_link_vacuum(unsigned int uid) const;
	void test_link_limit(unsigned int uid) const;
	void test_multiple_same_link(const Connection& c) const;
		

	// UID FUNCTIONS ----------------------------------------------	
	unsigned int next_uid{0}; 
	unsigned int get_next_uid();
	unsigned int next_cid{0};
	unsigned int get_next_cid();

	bool uid_already_exists(unsigned int id) const;

	// DATA STRUCTURE ----------------------------------------------
	map<unsigned int, Node> nodes_by_uid;
	map<unsigned int, vector<unsigned int>>
	outgoing_connections_id_by_uid;
	map<unsigned int, Connection> connections_by_id;

	// NESTED CLASSES -----------------------------------------------
	class Criteria
	{
	public:	
		void reset();
		Criteria_values get_criteria();
		
		// Add and delete contributions
		void operator+=(const Node &n);
		void operator-=(const Node &n);
		void operator+=(const Connection &c);
		void operator-=(const Connection &c);

				// ENJ
		void calculate_enj();
					
		// MTA
		void calculate_mta();
		// CI automatically updated in add and del connections.

	private:
		void node_contribution(const Node &n, const int sign);
		void connection_contribution(const Connection &c, const int sign); 

		// Measurements for criteria
		unsigned int housing_population=0;
		unsigned int other_population=0;

		Criteria_values criteria_values{0., 0., 0.};
	};

	Criteria criteria;

	class Dijkstra
	{
	public:
		// Getters
		vector<unsigned int>& get_shortest_path_nodes();
		vector<unsigned int>& get_shortest_path_connections();

		// Recalculating shortest path
		pair<double, double> run_algorithm(unsigned int start_uid, 
										   bool time_only = false);
		void invalidate_shortest_path();

	private:
		// Shortest path
		vector<unsigned int> shortest_path_nodes;
		vector<unsigned int> shortest_path_connections;

		// Internal data structure: ------------------------------------
		map<unsigned int, double> time_by_uid;
		map<unsigned int, unsigned int> parent_uid_by_uid;

		pair<unsigned int, double> fastest_production;
		pair<unsigned int, double>  fastest_transport;

		bool production_found;
		bool transport_found;

		// Internal methods : ---------------------------------------
		// Dijkstra
		bool exit_dijkstra(unsigned int stard_uid);
		void init_data_structure(unsigned int start_uid);
		pair<unsigned int, double> get_lowest_access_time();
		void update_neighbors(pair<unsigned int, double>& 
									  lowest_access_time);
				
		// Backtracking
		void fill_shortest_path_vector(unsigned int start_uid);
		void backtrack_path(unsigned int from, unsigned int to);
		void update_shortest_path_connections();
	};

	Dijkstra dijkstra;
};

#endif // HEADER_GUARD_VILLE_H
