
// Architecture: Fig 11 b1

// file: noeud.h
// author: Marc Bachelet, Michael Linder
// last edited: 24.03.2020
// status: tested
// to-do: -

#ifndef HEADER_GUARD_NOED_MLX11_01032020
#define HEADER_GUARD_NOED_MLX11_01032020

#include "tools.h"
#include "noeud_type.h"
#include "coordinates.h"

class Connection; 

class Node
{
public:
	Node(const unsigned int population, const Coord position,
		 const unsigned int uid=0);
		
	// Setters
	void set_population(int pop);
	void set_uid(unsigned int id);
	void set_position(Coord c);

	// Getters
	unsigned int get_population() const;
	double get_radius() const;
	unsigned int get_uid() const;
	Circle get_circle() const;

	bool fast_speed_compatible() const;
	int get_link_limit() const;
	Node_type get_type() const;
		
	// Distance functions
	double distance_to_node(const Node& other) const;
	double distance_to_connection(const Connection& c) const;

	// Drawing
	void on_draw(const Color color) const;
	
private:
		
	Circle representation;
	unsigned int uid;
	unsigned int population;
		
	// Internal function
	void update_radius();
		
protected:	
	bool fast_speed;
	int link_limit;		
	Node_type type;
};

// Subclasses of node:
class Housing: public Node
{
public:
	Housing(unsigned int population, Coord position, unsigned int uid=0);
};

class Production: public Node
{
public:
	Production(unsigned int population, Coord position, unsigned int uid=0);
};

class Transport: public Node
{
public:
	Transport(unsigned int population, Coord position,  unsigned int uid=0);
};


class Connection
{
public:
	Connection(const Node& A, const Node& B);

	// Getters
	unsigned int get_uid_of_point_A() const;
	unsigned int get_uid_of_point_B() const;
	double get_speed() const;
	double get_travel_time() const;
	Line get_line() const;		
	bool is_connected_to(unsigned int uid) const;

	// Drawing
	void on_draw(const Color color) const;

private:
	unsigned int uid_of_point_A;
	unsigned int uid_of_point_B;	
	double speed; 
	double travel_time;
	Line representation;
};

#endif
