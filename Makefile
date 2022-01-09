
# Define macros
CXX     = g++
CXXFLAGS = -g -O3 -Wall -std=c++11 `pkg-config --cflags gtkmm-3.0`
CXXFILES = projet.cc ville.cc noeud.cc tools.cc error.cc gui.cc visualisation.cc
GTKMM_FLAG = `pkg-config gtkmm-3.0 --cflags --libs`
OFILES = projet.o ville.o noeud.o tools.o error.o gui.o visualisation.o

# Compiles all files in program to create executable "prog"
projet: $(OFILES)
	$(CXX) $(CXXFLAGS) $(OFILES) -o projet  $(GTKMM_FLAG)

projet.o: projet.cc
	$(CXX) projet.cc -c  $(GTKMM_FLAG)

gui.o: gui.cc visualisation_gui.h coordinates.h
	$(CXX) gui.cc -c $(GTKMM_FLAG)

# Generates all depencies
depend:
	@echo " *** Update dependencies ***"
	@(sed '/^# DO NOT DELETE THIS LINE/q' Makefile && \
	  $(CXX) -MM $(CXXFLAGS) $(CXXFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

# Deletes all .o and executables.
clean:
	@echo " *** Cleaned .o files ***"
	@/bin/rm -f *.o *.x *.cc~ *.h~ projet

# DEPENDENCY RULES:
projet.o: projet.cc tools.h coordinates.h visualisation_color.h error.h \
 visualisation_gui.h visualisation_tools.h
ville.o: ville.cc noeud.h tools.h coordinates.h visualisation_color.h \
 constantes.h error.h ville.h
noeud.o: noeud.cc coordinates.h visualisation_color.h tools.h noeud.h \
 constantes.h error.h
tools.o: tools.cc tools.h coordinates.h visualisation_color.h \
 visualisation_tools.h
error.o: error.cc error.h constantes.h \
 visualisation_gui.h coordinates.h visualisation_tools.h \
 visualisation_color.h constantes.h
