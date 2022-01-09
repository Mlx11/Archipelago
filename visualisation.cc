// Architecture: Fig 11 b1

// file: visualisation.cc
// author: Marc Bachelet, Michael Linder
// last edited: 15.04.2020
// status: tested, styled
// to-do: -

#include "visualisation.h"
#include "visualisation_gui.h"
#include "visualisation_tools.h"
#include "constantes_visualisation.h"

// to convert between different sized drawing area and region of the city displayed
static double window_correction(1);
static Zoom zoom_factor(1);
static Zoom zoom(zoom_factor*window_correction);

static const Cairo::RefPtr<Cairo::Context>* cr(nullptr);
static Coord center_drawing_area{Coord{0,0}};

Coord get_coordinates_in_gui(const Coord c)
{
	Coord relative_coords;
	relative_coords.x = center_drawing_area.x + c.x*zoom;
	relative_coords.y = center_drawing_area.y - c.y*zoom;
	return relative_coords;
}


void set_color(const Color color)
{
	// default color is black
	switch(color)
	{
		case RED   : (*cr)->set_source_rgb(1,0,0); break;
		case GREEN : (*cr)->set_source_rgb(0,1,0); break;
		case WHITE : (*cr)->set_source_rgb(1,1,1); break;
		case BLACK : (*cr)->set_source_rgb(0,0,0); break;
		default: 	 (*cr)->set_source_rgb(0,0,0); break;
	}
}


void update_zoom()
{
	zoom = zoom_factor*window_correction;
}


void set_center_drawing_area(const Coord center)
{
	center_drawing_area = center;
}


void set_context(const Cairo::RefPtr<Cairo::Context>& context)
{
	cr = &context;
}



void initialize_zoom(const double dim_max, const double default_drawing_size, 
					 const Zoom default_zoom)

{
	window_correction = default_drawing_size / (2*dim_max);
	zoom_factor = default_zoom;
	update_zoom();
}


Zoom get_zoom()
{
	return zoom_factor;
}


void set_zoom(const Zoom z)
{
	zoom_factor = z;
	update_zoom();
}


void draw_cairo_background()
{
	(*cr)->save();
	(*cr)->set_source_rgb(1,1,1);
	(*cr)->rectangle(0,0, 2*center_drawing_area.x, 2*center_drawing_area.y);
	(*cr)->fill();
	(*cr)->stroke();
	(*cr)->restore();
}


Coord get_coordinates_in_model(const Coord c)
{
	Coord model_coords;
	model_coords.x = (c.x - center_drawing_area.x)/zoom;
	model_coords.y = (-c.y + center_drawing_area.y)/zoom;
	return model_coords;
}


void draw_line(const Coord c1, const Coord c2, const Color color)
{
	Coord c1_gui = get_coordinates_in_gui(c1);
	Coord c2_gui = get_coordinates_in_gui(c2);
	
	(*cr)->save();
	(*cr)->set_line_width(line_width*zoom);
	set_color(color);
	(*cr)->move_to(c1_gui.x, c1_gui.y);
	(*cr)->line_to(c2_gui.x, c2_gui.y);
	(*cr)->stroke();
	(*cr)->restore();
}


void draw_circle(const Coord center, const double radius, const Color color)
{
	Coord circle_center_gui = get_coordinates_in_gui(center);
	
	// First draw a white circle as background (covers lines that are there)
	(*cr)->save();
	set_color(WHITE);
	(*cr)->arc(circle_center_gui.x, circle_center_gui.y, radius*zoom, 0.0, 2.0*M_PI);
	(*cr)->fill_preserve();
	(*cr)->stroke();
	(*cr)->restore();
	
	// Draw the circle itself
	(*cr)->save();
	(*cr)->set_line_width(line_width*zoom);
	set_color(color);
	(*cr)->arc(circle_center_gui.x, circle_center_gui.y, radius*zoom, 0.0, 2.0*M_PI);
	(*cr)->stroke();
	(*cr)->restore();
}


void draw_rectangle(const Coord center_model, const double height, const double width,
					const Color color)
{
	Coord center = get_coordinates_in_gui(center_model);
	(*cr)->save();
	(*cr)->set_line_width(line_width*zoom);
	set_color(color);
	(*cr)->rectangle(center.x - width/2*zoom, center.y - height/2*zoom,
					 width*zoom, height*zoom); 
	(*cr)->stroke();
	(*cr)->restore();
}


void draw_asterix(const Coord center, const double radius, const Color color)
{
	// Name of the edge points of the asterix lying in a circle
	/*          a
	 *    i     |     b
	 * 		\   |   /
	 *  h------ * ------d
	 *      /   |    \   
	 *    g     |       e
	 *          f
	 * 
	 *  * is center
	 */      
	Coord a{center.x, center.y + radius};
	Coord b{center.x + radius/sqrt(2), center.y + radius/sqrt(2)};
	Coord d{center.x + radius, center.y};
	Coord e{center.x + radius/sqrt(2), center.y - radius/sqrt(2)};
	Coord f{center.x, center.y-radius};
	Coord g{center.x - radius/sqrt(2), center.y - radius/sqrt(2)};
	Coord h{center.x - radius, center.y};
	Coord i{center.x - radius/sqrt(2), center.y + radius/sqrt(2)};
	
	draw_line(a, f, color);
	draw_line(b, g, color);
	draw_line(d, h, color);
	draw_line(e, i, color);
}

