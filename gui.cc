
// Architecture: Fig 11 b1

// file: Gui.cc
// author: Marc Bachelet, Michael Linder
// last edited: 14.04.2020
// status: tested
// to-do: -

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>


#include "city_gui.h"
#include "gui.h"
#include "constantes.h"
#include "error.h"
#include "coordinates.h"
#include "constantes_gui.h"

// The maximum distance the mouse moves between click and release to count as a click
#define MAX_MOUSE_MOVE_DIST_FOR_CLICK 5
#define SMALL_NUMBER pow(10, -6)

using namespace std;

Gui::Gui(char* filename) :
	drawing_area(this),
	button_exit(string_button_exit),
	button_new(string_button_new),
	button_open(string_button_open),
	button_save(string_button_save),
	button_shortest_path(string_button_shortest_path),
	button_zoom_in(string_button_zoom_in),
	button_zoom_out(string_button_zoom_out),
	button_zoom_reset(string_button_zoom_reset),
	button_edit_link(string_button_edit_link),
	radioButton_housing(string_name_housing),
	radioButton_transportation(string_name_transportation),
	radioButton_production(string_name_production),
	box_radio_buttons(Gtk::ORIENTATION_VERTICAL),
	box_general(Gtk::ORIENTATION_VERTICAL),
	box_display(Gtk::ORIENTATION_VERTICAL),
	box_editor(Gtk::ORIENTATION_VERTICAL),
	box_informations(Gtk::ORIENTATION_VERTICAL),
	box_menu(Gtk::ORIENTATION_VERTICAL),
	box_window(Gtk::ORIENTATION_HORIZONTAL),
	frame_general(string_frame_general),
	frame_display(string_frame_display),
	frame_editor(string_frame_editor),
	frame_informations(string_frame_informations)
{
	add_events(Gdk::KEY_RELEASE_MASK);
	connect_signals();
	group_radios();
	pack_gui();

	// open city if argument argv[1] has been passed
	if (filename != 0) 
	{
		open_file(filename);
	}

	// Init zoom
	initialize_zoom(dim_max, default_drawing_size, default_zoom);
	update_criteria();
	set_zoom_label();
}


void Gui::group_radios()
{
	radioButton_transportation.join_group(radioButton_housing);
	radioButton_production.join_group(radioButton_housing);
}


void Gui::connect_signals()
{
	// connect signals
	button_exit.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_exit_clicked));
	button_new.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_new_clicked));
	button_open.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_open_clicked));
	button_save.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_save_clicked));
	button_zoom_in.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_zoom_in_clicked));
	button_zoom_out.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_zoom_out_clicked));
	button_zoom_reset.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_zoom_reset_clicked));
	button_shortest_path.signal_clicked().
		connect(sigc::mem_fun(*this, &Gui::on_button_shortest_path_clicked));
}


void Gui::pack_gui()
{	
	// pack the sub-boxes
	box_general.pack_start(button_exit, FALSE, FALSE, margin);
	box_general.pack_start(button_new, FALSE, FALSE, margin);
	box_general.pack_start(button_open, FALSE, FALSE, margin);
	box_general.pack_start(button_save, FALSE, FALSE, margin);
	box_display.pack_start(button_shortest_path, FALSE, FALSE, margin);
	box_display.pack_start(button_zoom_in, FALSE, FALSE, margin);
	box_display.pack_start(button_zoom_out, FALSE, FALSE, margin);
	box_display.pack_start(button_zoom_reset, FALSE, FALSE, margin);
	box_display.pack_start(label_zoom, FALSE, FALSE, margin);
	box_editor.pack_start(button_edit_link, FALSE, FALSE, margin);
	box_editor.pack_start(box_radio_buttons, FALSE, FALSE, margin);
	box_informations.pack_start(label_enj, FALSE, FALSE, margin);
	box_informations.pack_start(label_ci, FALSE, FALSE, margin);
	box_informations.pack_start(label_mta, FALSE, FALSE, margin);
	box_radio_buttons.pack_start(radioButton_housing, FALSE, FALSE, margin);
	box_radio_buttons.pack_start(radioButton_transportation, FALSE, FALSE, margin);
	box_radio_buttons.pack_start(radioButton_production, FALSE, FALSE, margin);
	
	// pack frames with the sub-boxes
	frame_general.add(box_general);
	frame_display.add(box_display);
	frame_editor.add(box_editor);
	frame_informations.add(box_informations);

	//pack menu box with frames
	box_menu.pack_start(frame_general, FALSE, TRUE, margin);
	box_menu.pack_start(frame_display, FALSE, TRUE, margin);
	box_menu.pack_start(frame_editor, FALSE, TRUE, margin);
	box_menu.pack_start(frame_informations, FALSE, TRUE, margin);

	//pack window box
	box_window.pack_start(box_menu, Gtk::PACK_SHRINK, Gtk::PACK_SHRINK);
	box_window.pack_start(drawing_area);
	add(box_window);

	show_all_children();
}


Gui::~Gui()
{
}


// ------------------------ Button events ---------------------------------------------

void Gui::on_button_exit_clicked()
{
	exit(0);
}


void Gui::on_button_new_clicked()
{
	// Reset data structure
	City_gui::reset();
	
	// Redraw
	drawing_area.queue_draw();
}


void Gui::on_button_open_clicked()
{	
	Gtk::FileChooserDialog dialog(string_dialog_filechooser,
								  Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Open", Gtk::RESPONSE_OK);

	// Display dialog
	int result = dialog.run();

	// Only read_file when "open" clicked
	if(result == Gtk::RESPONSE_OK)
	{
		std::string filename = dialog.get_filename();
		char* filename_cp = const_cast<char*>(filename.c_str());
		open_file(filename_cp);
	}
}


void Gui::show_error_dialog(const string text)
{
	Gtk::MessageDialog dialog(*this, string_dialog_error_title, false,
							  Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
	dialog.set_secondary_text(text);
	dialog.run(); // This returns a response which we do not need here.
}


void Gui::on_button_save_clicked()
{
	Gtk::FileChooserDialog dialog(string_dialog_file_save,
								  Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);

	// Display dialog
	int result = dialog.run();

    // Saving into file if save clicked.
	if (result== Gtk::RESPONSE_OK)
	{
		std::string filename = dialog.get_filename();
		save_file(filename);
	}
}


void Gui::on_button_zoom_in_clicked()
{
	std::cout << "Zoom in clicked" << std::endl;
	Zoom zoom = get_zoom();
	zoom += delta_zoom;

	if(zoom > max_zoom + SMALL_NUMBER)
	{
		show_error_dialog(error::max_zoom_reached(max_zoom));
		zoom = max_zoom;
	}

	set_zoom(zoom);
	drawing_area.queue_draw();
	set_zoom_label();
}


void Gui::on_button_zoom_out_clicked()
{
	std::cout << "Zoom out clicked" << std::endl;
	Zoom zoom = get_zoom();
	zoom -= delta_zoom;
	if(zoom < min_zoom)
	{
		if(zoom < min_zoom - SMALL_NUMBER)
		{
			show_error_dialog(error::min_zoom_reached(min_zoom));
		}
		zoom = min_zoom;
	}
	set_zoom(zoom);
	drawing_area.queue_draw();
	set_zoom_label();
}


void Gui::on_button_zoom_reset_clicked()
{
	std::cout << "Zoom reset clicked" << std::endl;
	set_zoom(default_zoom);
	drawing_area.queue_draw();
	set_zoom_label();
}


void Gui::on_button_shortest_path_clicked()
{
	std::cout << "shortest path toggled" << std::endl;
	drawing_area.queue_draw();
}

void Gui::open_file(const char* filename)
{
	try
	{
		City_gui::reset();
		Filereader filereader;
		filereader.read_file(filename);
	}
	// in case of the file containing errors
	catch(std::string err)
	{
		if(err != error::success())
		{
			City_gui::reset();
			show_error_dialog(err);
		}
	}
	drawing_area.queue_draw();
}

void Gui::save_file(string filename)
{
	Filewriter writer;
	std::ofstream file;

	file.open(filename);
	file << writer.write();
	file.close();
}


// -------------------------- Get states of buttons -----------------------------------

Node_type Gui::get_selected_node_type() const 
{
	if(radioButton_transportation.get_active()) {return TRANSPORT;}
	if(radioButton_production.get_active()) {return PRODUCTION;}
	return HOUSING;
}

bool Gui::get_shortest_path_active() const
{
	return button_shortest_path.get_active();
}

// ----------------------------- Mouse/Key Events -------------------------------------

bool Gui::on_mouse_click_in_drawing_area(const GdkEventButton *event)
{
	double x=event->x;
	double y=event->y;
	Coord c_gui{x,y};
	Coord c_model= get_coordinates_in_model(c_gui);
	
	try{
		// left mouse button
		if(event->button == 1)
		{
			left_mousclick(c_model);
		}
		// right mouse button
		else if (event->button == 3)
		{
			City_gui::move_selected_node_to_coord(c_model);
		}
	// catching errors thrown by non authorized modifications
	} catch(std::string err)
	{
		std::cerr << err;
		show_error_dialog(err);
	}
	//~ update_criteria();
	drawing_area.queue_draw();
    return true;
}


bool Gui::left_mousclick(Coord click)
{
	unsigned int new_selected_uid(City_gui::get_node_uid_near_coords(click));
	unsigned int selected_node_uid(City_gui::get_selected_node_uid());
	
	if(new_selected_uid != no_link) // if not outside of a node
	{
		//if second click on the node
		if (selected_node_uid == new_selected_uid)
		{
			City_gui::delete_selected_node();
			City_gui::recalculate_criteria();
		}
		else if((selected_node_uid != no_link) && button_edit_link.get_active())
		{ // if an other node was selected and edit link is active 
			City_gui::create_new_connection(new_selected_uid);
		}
		else // if an other node was selected and edit link is inactive
		{
			City_gui::select_new_node(new_selected_uid);
		}
	}
	else // if outside of node
	{
		if(selected_node_uid != no_link) // if a node had been selected priorly
		{
			City_gui::select_new_node(no_link);
		}
		else // if no node was selected
		{
			City_gui::create_new_node(get_selected_node_type(), click);
			City_gui::recalculate_criteria();	
		}
	}
    return true;
}


bool Gui::on_key_release_event(GdkEventKey* event)
{
	/* Key inputs:
	 * i: zoom in
	 * o: zoom out
	 * r: reset zoom
	 * esc: end program
	 * */
	 
	switch (event->keyval)
	{
	case GDK_KEY_i: on_button_zoom_in_clicked(); return true;
	case GDK_KEY_o: on_button_zoom_out_clicked(); return true;
	case GDK_KEY_r: on_button_zoom_reset_clicked(); return true;
	case GDK_KEY_Escape: on_button_exit_clicked(); return true;
	default: return false;
	} 
}


//---------------------- Criteria -----------------------------------------------------

void Gui::update_criteria()
{
	Criteria_values values{City_gui::get_criteria()};
	set_enj_label(values.enj);
	set_mta_label(values.mta);
	set_ci_label(values.ci);
}


//--------------------- set labels ---------------------------------------------------

void Gui::set_zoom_label(){
	std::ostringstream ss;
	ss << std::setprecision(3);
	ss << string_label_zoom << get_zoom();
	label_zoom.set_text(ss.str());
}


void Gui::set_enj_label(Zoom enj){
	std::ostringstream ss;
	ss << std::setprecision(5);
	ss << string_label_enj << enj;
	label_enj.set_text(ss.str());
}


void Gui::set_ci_label(double ci){
	std::ostringstream ss;
	ss << std::setprecision(6);
	ss << string_label_ci << ci;
	label_ci.set_text(ss.str());
}


void Gui::set_mta_label(double mta){
	std::ostringstream ss;
	ss << std::setprecision(6);
	ss << string_label_mta << mta;
	label_mta.set_text(ss.str());
}


// ---------------------- Drawing Area ------------------------------------------------

DrawingArea::DrawingArea(Gui* gui) : Gtk::DrawingArea(), parent(gui)
{
	// Set masks for mouse events - the key events are in gui
    add_events(Gdk::BUTTON_PRESS_MASK);
    add_events(Gdk::BUTTON_RELEASE_MASK);
    
    // Set initial size - this is canceled after the first draw
    set_size_request(default_drawing_size, default_drawing_size);
}


bool DrawingArea::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	static bool first_draw{true};
	
	// give the necesary informations to visualisation
	set_context(cr);
	set_center_drawing_area(get_center_coords());

	// in case of the first draw delete the initial size constrainte
	if (first_draw)
	{
		set_size_request(0,0);
		first_draw = false;
	}
	
	parent->update_criteria();
	
	draw_cairo_background();

	City_gui::draw_city(parent->get_shortest_path_active());
	
	return true;
}

	
Coord DrawingArea::get_center_coords() const
{
	Gtk::Allocation allocation = get_allocation();
	const double width = allocation.get_width();
	const double height = allocation.get_height();
	
	//center
	double x_center{width/2};
	double y_center{height/2};
	Coord center{x_center, y_center};
	return center;
}
	
// ------------------ Events ----------------------------------------------------------

bool DrawingArea::on_button_press_event(GdkEventButton *event)
{
	last_left_click_position.x = event->x;
	last_left_click_position.y = event->y;
	return true;
}


bool DrawingArea::on_button_release_event(GdkEventButton *event)
{
	double mouse_mouvement_x = pow(last_left_click_position.x - event->x,2);
	double mouse_mouvement_y = pow(last_left_click_position.y - event->y,2);

	// check for click or drag
	if((mouse_mouvement_x < pow(MAX_MOUSE_MOVE_DIST_FOR_CLICK, 2))
		&& (mouse_mouvement_y < pow(MAX_MOUSE_MOVE_DIST_FOR_CLICK, 2)))
	{
		return parent->on_mouse_click_in_drawing_area(event);
	}
	else // mouse moved between clicks
	{
		Coord click_1_model = get_coordinates_in_model(last_left_click_position);
		Coord click_2_model = get_coordinates_in_model(Coord{event->x, event->y});
		
		try
		{
			City_gui::change_size_of_node(click_1_model, click_2_model);
		}
		catch(string err)
		{
			parent->show_error_dialog(err);
		}
		queue_draw();
	}
}
