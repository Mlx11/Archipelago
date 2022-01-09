
// Architecture: Fig 11 b1

// file: visualisation_tools.h
// author: Marc Bachelet, Michael Linder
// last edited: 25.04.2020
// status: tested, styled
// to-do: -


#ifndef HEADER_GUARD_VISUALISATION_TOOLS_H
#define HEADER_GUARD_VISUALISATION_TOOLS_H

#include "visualisation_color.h"
#include "coordinates.h"

void draw_line(const Coord c1, const Coord c2, const Color color);
void draw_circle(const Coord center, const double radius, const Color color);
void draw_rectangle(const Coord center, const double height, const double width,
					const Color color);
void draw_asterix(const Coord center_model, const double radius, const Color color);

#endif //HEADER_GUARD_VISUALISATION_TOOLS_H
