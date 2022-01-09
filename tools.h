
// Architecture: Fig 11 b1

// file: tools.h
// author: Marc Bachelet, Michael Linder
// last edited: 26.03.2020
// status: tested, styled
// to-do: -

#ifndef HEADER_GUARD_TOOLS_H
#define HEADER_GUARD_TOOLS_H

#include "coordinates.h"
#include "visualisation_color.h"


class R2Vector
{
public:
	explicit R2Vector(const Coord c);
	double norm() const;
	double get_x() const;
	double get_y() const;

private:
	double x;
	double y;
};


// Vector operations:
const double operator * (const R2Vector& u, const R2Vector& v);
const R2Vector operator + (const R2Vector& u, const R2Vector& v);
const R2Vector operator - (const R2Vector& u, const R2Vector& v);
const R2Vector operator * (const double& c, const R2Vector& v);


class Line
{
public:
	Line(const Coord point_A={0,0}, const Coord point_B={0,0});
		
	// Getters and setters:
	Coord get_point_A() const;
	Coord get_point_B() const;
	void set_point_A(const Coord A);
	void set_point_B(const Coord B);
	double get_length();
	R2Vector get_line_as_vector() const; // Vector from point A to B

	//drawing
	void on_draw(const Color color) const;	

private:
	Coord point_A;
	Coord point_B;
	double length;
	double calculate_length() const; 
};

	
class Circle
{
public:
	explicit Circle(const Coord c={0,0}, const double r=0);

	// Getters and setters
	Coord get_center_coordinates() const;
	void set_center_coordinates(Coord c);
	double get_radius() const;
	void set_radius(double r);

	// Distance functions
	double dist_to_circle(Circle other) const;
	double dist_to_line(Line l) const;	

	// drawing
	void on_draw(const Color color) const;

private:
	Coord center;
	double radius;
};

class Rectangle
{
public:
	Rectangle(Coord center, double width, double height);

	// Drawing:
	void on_draw(const Color color) const;

private:
	Coord center;
	double width;
	double height;
};

class Asterix
{
public:
	Asterix(Coord center, double radius);

	// Drawing:
	void on_draw(const Color color) const;
	
private:
	Coord center;
	double radius;
};

#endif // HEADER_GUARD_TOOLS_H
