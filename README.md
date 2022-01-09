# Archipelago

A project made by Marc Bachelet and Michael Linder at EPFL Lausanne under Prof. Ronan Boulic. 

This is a backup repository. The original repository is hosted by Marc: https://github.com/MBachelet

## Architecture
The architecture of our program is showed in the diagram below
![text][uml]
<br/> **Constantes**: We have multiple files with constantes: constantes_gui.h providing all the strings used in the gui, constantes_visualisation having all necessary constantes for the drawing and constantes.h containing all the other constantes used by the model.
<br/>**GUI**: The GUI is made out of two classes Gui and DrawingArea which communicate with one another. 
<br/>**City**: This is the biggest level hosting the class City_gui which acts as a manager of the instance of city and the class City itself. Dijkstra and Criteria are nested in the city. Additionaly we habe a filereader and filewriter automate at the city level. 
<br/>**Node, Tools**: nothing special
<br/>**Visualisation**: Visualisation is split into two header files with the functions visible to tools (to draw stuff) and gui (mostly to initialize values).











[uml]: https://github.com/MBachelet/Archipelago/blob/master/Diagramm_Archipelago.png
