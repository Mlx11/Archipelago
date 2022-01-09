
// Architecture: Fig 11 b1

// file: Gui.h
// author: Marc Bachelet, Michael Linder
// last edited: 14.04.2020
// status: tested
// to-do: -

#ifndef HEADER_GUARD_Gui_H
#define HEADER_GUARD_Gui_H

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/window.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <chrono>

#include "noeud_type.h"
#include "visualisation_gui.h"

class Gui;

class DrawingArea : public Gtk::DrawingArea
{
public:
	DrawingArea(Gui* gui);
  
protected: 
	bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

	Coord get_center_coords() const;

	// Events
	bool on_button_press_event(GdkEventButton *event) override;
	bool on_button_release_event(GdkEventButton *event) override;

private:
	
	Gui* parent;
	Coord last_left_click_position;
};


class Gui : public Gtk::Window
{
public:
	Gui(char* filename);
	virtual ~Gui();

	// getter
	bool get_shortest_path_active() const;

	// Events
	bool on_mouse_click_in_drawing_area(const GdkEventButton *event);

	// display stuff
	void show_error_dialog(const std::string text);
	void update_criteria();

protected:
	//Signal handlers:
	void on_button_exit_clicked();
	void on_button_new_clicked();
	void on_button_open_clicked();
	void on_button_save_clicked();
	void on_button_shortest_path_clicked();
	void on_button_zoom_in_clicked();
	void on_button_zoom_out_clicked();
	void on_button_zoom_reset_clicked();
	void on_button_edit_link_clicked() const;
	void on_radio_type_clicked() const;

	// Keyboard input
	bool on_key_release_event(GdkEventKey* event) override;

	// Cairo
	DrawingArea drawing_area;

	// Buttons
	Gtk::Button button_exit;
	Gtk::Button button_new;
	Gtk::Button button_open;
	Gtk::Button button_save;
	Gtk::ToggleButton button_shortest_path; 
	Gtk::Button button_zoom_in;
	Gtk::Button button_zoom_out;
	Gtk::Button button_zoom_reset;
	Gtk::ToggleButton button_edit_link;
	
	// Labels
	Gtk::Label label_zoom;
	Gtk::Label label_enj;
	Gtk::Label label_ci;
	Gtk::Label label_mta;
	
	// Radio buttons
	Gtk::RadioButton radioButton_housing;
	Gtk::RadioButton radioButton_transportation;
	Gtk::RadioButton radioButton_production;
	// Structure
	Gtk::Box box_radio_buttons;
	Gtk::Box box_general;
	Gtk::Box box_display;
	Gtk::Box box_editor;
	Gtk::Box box_informations;
	Gtk::Box box_menu;
	Gtk::Box box_window;
	Gtk::Frame frame_general;
	Gtk::Frame frame_display;
	Gtk::Frame frame_editor;
	Gtk::Frame frame_informations;
	
private:
	//constructor help functions
	void group_radios();
	void pack_gui();
	void connect_signals();

	// intermediate steps
	bool left_mousclick(const Coord click);
	void open_file(const char* filename);
	void save_file(std::string filename);

	// Read radio buttons
	Node_type get_selected_node_type() const;

	// Set label texts
	void set_zoom_label();
	void set_enj_label(const Zoom enj);
	void set_ci_label(const double ci);
	void set_mta_label(const double mta);
};

#endif  // HEADER_GUARD_Gui_H
