
// Architecture: Fig 11 b1

// file: tools.cc
// author: Marc Bachelet, Michael Linder
// last edited: 01.04.2020
// status: tested
// to-do: -

#include "tools.h"
#include "visualisation_tools.h"

#include <cmath>


	
// ------------------R2Vector-------------------------------------------
	
R2Vector::R2Vector(const Coord c)
: x(c.x), y(c.y)
{
}

	
double R2Vector::get_x() const
{
	return x;
}

	
double R2Vector::get_y() const
{
	return y;
}

	
double R2Vector::norm() const
{
	return sqrt(pow(x,2) + pow(y,2));
}


// --------------- operators ------------------------------------------
	
const double operator * (const R2Vector& u, R2Vector& v)
{
	return u.get_x()*v.get_x() + u.get_y()*v.get_y();
}

	
const R2Vector operator + (const R2Vector& u, const R2Vector& v)
{
	Coord c{u.get_x()+v.get_x(), u.get_y()+v.get_y()};
	return R2Vector(c);
}

	
const R2Vector operator - (const R2Vector& u, const R2Vector& v)
{
	Coord c{u.get_x()-v.get_x(), u.get_y()-v.get_y()};
	return R2Vector(c);
}

	
const R2Vector operator * (const double& c, const R2Vector& v)
{
	Coord coordinate{c*v.get_x(), c*v.get_y()};
	return R2Vector(coordinate);
}



//--------------------Line----------------------------------------------

Line::Line(const Coord point_A, const Coord point_B)
: point_A(point_A), point_B(point_B), length(-1)
{
}


Coord Line::get_point_A() const
{
	return point_A;
}


Coord Line::get_point_B() const
{
	return point_B;
}	


void Line::set_point_A(const Coord A)
{
	point_A = A;
}


void Line::set_point_B(const Coord B)
{
	point_B = B;
}


double Line::get_length() 
{
	if (length == -1)
	{
		length = calculate_length();
	}
	return length;	
}

	
double Line::calculate_length() const 
{
	return get_line_as_vector().norm();
}

	
R2Vector Line::get_line_as_vector() const
{
	return R2Vector(point_B) - R2Vector(point_A);
}


void Line::on_draw(const Color color) const 
{
	draw_line(point_A, point_B, color);
}

	
//--------------------Circle--------------------------------------------
	
Circle::Circle(const Coord c, const double r)
: center(c), radius(r)
{
}


double Circle::get_radius() const
{
	return radius;
}


void Circle::set_radius(double r)
{
	radius = r;
}


Coord Circle::get_center_coordinates() const
{
	return center;
}


double Circle::dist_to_circle(Circle other) const
{
	R2Vector dif = R2Vector(center) - R2Vector(other.center);
	return dif.norm() -(other.radius + radius);
}


double Circle::dist_to_line(Line l) const
{
	R2Vector circle_center = R2Vector(center);
	R2Vector line_start = R2Vector(l.get_point_A());
	
	// Test if the line is a point
	if (l.get_length() == 0) // precludes zero division
	{
		Coord tmp = l.get_point_A();
		return dist_to_circle(Circle(tmp));
	}

	// line_start is considered as Origin (0,0)
	// rel means position/vector relative to line_start 
	R2Vector rel_line_end = l.get_line_as_vector();
	R2Vector rel_circle_center = circle_center - line_start;
	double sp{rel_line_end * rel_circle_center};  // scalar product
	if(sp<0) // A is the nearest point to the circle center
	{
		return rel_circle_center.norm() - get_radius();
	}
		
	double proj_circle_onto_line_length = sp / rel_line_end.norm();
	
	// B is the closest point to the circle center
	if (proj_circle_onto_line_length > rel_line_end.norm()) 
	{ 
		return (rel_line_end - rel_circle_center).norm()-get_radius();
	}
		
	// The closest point to the circle center is on the line
	double distance = sqrt(pow(rel_circle_center.norm(), 2) 
					  - pow(proj_circle_onto_line_length, 2));
					  
	return distance - radius;
}


void Circle::set_center_coordinates(Coord c)
{
	center = c;
}


void Circle::on_draw(const Color color) const
{
	draw_circle(center, radius, color);
}


// --------------------Rectangle-----------------------------------------------

Rectangle::Rectangle(Coord center, double width, double height) :
	center(center),
	width(width),
	height(height)
{
}

void Rectangle::on_draw(const Color color) const
{
	draw_rectangle(center, width, height, color);
}

// ---------------------Asterix------------------------------------------------

Asterix::Asterix(Coord center, double radius) : center(center), radius(radius)
{
}

void Asterix::on_draw(const Color color) const 
{
	draw_asterix(center, radius, color);
}
