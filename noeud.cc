
// Architecture: Fig 11 b1

// file: noeud.cc
// author: Marc Bachelet, Michael Linder
// last edited: 24.03.2020
// status: tested
// to-do: -

#include "noeud.h"
#include "error.h"
#include "constantes.h"

#include <cmath>


// ---------------------NODE----------------------------------------------------

Node::Node(const unsigned int population, const Coord position, 
		   const unsigned int uid)

: uid(uid), population(population), fast_speed(false), link_limit(no_link), 
  type(HOUSING)

{
	// test for reserved_uid
	if(uid==no_link)
	{
		throw std::string(error::reserved_uid());
	}

	// test for too_little_capacity
	if(population < min_capacity)
	{
		throw std::string(error::too_little_capacity(population));
	}

	// test for too_much_capacity
	if(population > max_capacity)
	{
		throw std::string(error::too_much_capacity(population));
	}

	representation = Circle(position, 0);
	update_radius();
}

// Getters

void Node::set_population(int pop)
{
	population = pop; 

	update_radius();
}


void Node::set_uid(unsigned int id)
{
	uid = id;
}


void Node::set_position(const Coord c)
{
	representation.set_center_coordinates(c);
}

// Getters

unsigned int Node::get_population() const
{
	return population;
}


double Node::get_radius() const
{
	return representation.get_radius();
}


unsigned int Node::get_uid() const
{
	return uid;
}


Circle Node::get_circle() const
{
	return representation;
}


bool Node::fast_speed_compatible() const
{
	return fast_speed;
}


int Node::get_link_limit() const
{
	return link_limit;
}


Node_type Node::get_type() const
{
	return type;
}

// Distance functions

double Node::distance_to_node(const Node& other) const
{
	return representation.dist_to_circle(other.get_circle());
}


double Node::distance_to_connection(const Connection& c) const
{
	return representation.dist_to_line(c.get_line());	
}

// Drawing

void Node::on_draw(const Color color) const
{
	representation.on_draw(color);

	if(get_type() == PRODUCTION)
	{
		Rectangle rect(representation.get_center_coordinates(),
					   2*representation.get_radius()/8,
					   2*representation.get_radius()*0.75);
		rect.on_draw(color);
	}
	else if(get_type() == TRANSPORT)
	{
		Asterix asterix(representation.get_center_coordinates(),
						representation.get_radius());
		asterix.on_draw(color);
	}
}


// Internal function

void Node::update_radius()
{
	representation.set_radius(sqrt(population));
}

//-----------------SUBCLASSES OF NODE-------------------------------------------


Housing::Housing(unsigned int population, Coord position, unsigned int uid)
: Node(population, position, uid)
{
	type = HOUSING;
	link_limit = max_link;
}


Transport::Transport(unsigned int population, Coord position, unsigned int uid)
: Node(population, position, uid)
{
	type = TRANSPORT;
	fast_speed= true;
}


Production::Production(unsigned int population, Coord position, unsigned int uid)
: Node(population, position, uid)
{
	type = PRODUCTION;
}


//--------------CONNECTION------------------------------------------------------

Connection::Connection(const Node &A, const Node &B)

: uid_of_point_A(A.get_uid()), uid_of_point_B(B.get_uid())
{
	// If both nodes are transportation, speed is set to fast_speed.
	if (A.fast_speed_compatible() and B.fast_speed_compatible())
	{
		speed = fast_speed;
	} 
	else 
	{
		speed = default_speed;
	}
				
	// Get node center coordinates
	Coord center_A(A.get_circle().get_center_coordinates());
	Coord center_B(B.get_circle().get_center_coordinates());

	representation = Line(center_A, center_B);

	// Get travel_time
	double dist = representation.get_length();
	travel_time = dist / speed;
}

// Getters	

unsigned int Connection::get_uid_of_point_A() const
{
	return uid_of_point_A;
}
	

unsigned int Connection::get_uid_of_point_B() const
{
	return uid_of_point_B;
}


double Connection::get_speed() const
{
	return speed;	
}


double Connection::get_travel_time() const
{
	return travel_time;
	
}


Line Connection::get_line() const
{
	return representation;
}
	

bool Connection::is_connected_to(unsigned int uid) const
{
	if((uid_of_point_A== uid) or (uid_of_point_B==uid))
	{
		return true;
	}
	return false;
}

// Drawing

void Connection::on_draw(const Color color) const
{
	representation.on_draw(color);
}



