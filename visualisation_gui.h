
// Architecture: Fig 11 b1

// file: visualisation_gui.h
// author: Marc Bachelet, Michael Linder
// last edited: 25.04.2020
// status: tested, styled
// to-do: -

#ifndef HEADER_GUARD_VISUALISATION_GUI_H
#define HEADER_GUARD_VISUALISATION_GUI_H

#include <cairomm/context.h>
#include "coordinates.h"

typedef double Zoom;

void set_center_drawing_area(const Coord center);
void set_context(const Cairo::RefPtr<Cairo::Context>& context);


void initialize_zoom(const double dim_max, const double default_drawing_size, 
                     const Zoom default_zoom);
Zoom get_zoom();
void set_zoom(const Zoom z);

void draw_cairo_background();

Coord get_coordinates_in_model(const Coord c);

#endif // HEADER_GUARD_VIUALISATION_GUI_H
