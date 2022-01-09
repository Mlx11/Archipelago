
// Architecture: Fig 11 b1

// file: city_gui.h
// author: Marc Bachelet, Michael Linder
// last edited: 14.04.2020
// status: tested
// to-do: -

#ifndef HEADER_GUARD_Ville_Gui_H
#define HEADER_GUARD_Ville_Gui_H

#include "ville.h"

 // Enum state keeps track of type of data read in function "read_line"
enum State {NBHOUSING, HOUSING_DATA, NBTRANSPORT, TRANSPORT_DATA, 
				NBPRODUCTION, PRODUCTION_DATA, NBLINK, LINK, FINISH};

enum Environment{gui, file};


class City_gui
{
public:
	// Modifying city through Gui:
	static void reset();

	static void create_new_connection(const unsigned int target_uid);
	static void create_new_node(const Node_type type, const Coord position);

	static void delete_selected_node();
	static void delete_connection_to_node(const unsigned int target_uid);	

	static void move_selected_node_to_coord(const Coord c);
		

	static void change_size_of_node(const Coord click_begin, 
									const Coord click_end);
	
	// Node selection:
	static void select_new_node(const unsigned int new_uid);
	static unsigned int get_selected_node_uid();

	// Getter for gui:
	static unsigned int get_node_uid_near_coords(const Coord c);

	// Drawing:
	static void draw_city(const bool show_shortest_path);

	// Criteria:
	static void recalculate_criteria();
	static Criteria_values get_criteria();

};


class Filereader
{	
public:
	void read_file(const char* filename);

private:
	// Internal reading functions
	void read_line(const std::string line);
	void read_number(std::istringstream& data);
	template<class T> 
	void read_node(std::istringstream& data);
	void read_link(std::istringstream& data);	

	// Reading variables:
	int read_state{0};
	unsigned int counter{0};
	unsigned int total{0};
};
	
	
class Filewriter
{
public:
	std::string write();	

private:
	void write_all_nodes();
	void write_node(const Node &n);
	void write_all_connections();

	std::string all;

	// Internal data structure:
	std::string str_housing;
	std::string str_transport;
	std::string str_production;
	unsigned int ctr_housing = 0;
	unsigned int ctr_transport = 0;
	unsigned int ctr_production = 0;
};

#endif // HEADER_GUARD_Ville_Gui_H
