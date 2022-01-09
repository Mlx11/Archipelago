
// Architecture: Fig 11 b1

// file: ville.cc
// author: Marc Bachelet, Michael Linder
// last edited: 24.03.2020
// status: tested
// to-do: -
// note: set gui instructions as comment.

#include <iostream>
#include <iomanip>

#include "gui.h"

using namespace std; 

int main(int argc, char * argv[])
{
	cout << setprecision(6) << fixed;

	if(argc > 2)
	{
		cout << "Too many arguments" << endl;
		return 0;
	}
	
	auto app = Gtk::Application::create("org.gtkmm.example");
	Gui gui(argv[1]);
	return app->run(gui);

}


