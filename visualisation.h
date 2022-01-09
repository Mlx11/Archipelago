
// Architecture: Fig 11 b1

// file: visualisation.h
// author: Marc Bachelet, Michael Linder
// last edited: 14.04.2020
// status: tested, styled
// to-do: -

#ifndef HEADER_GUARD_VISUALISATION_H
#define HEADER_GUARD_VISUALISATION_H

#include "coordinates.h"
#include "visualisation_color.h"

Coord get_coordinates_in_gui(Coord c);
void set_color(const Color color);
void update_zoom();

#endif // HEADER_GUARD_VISUALISATION_H
