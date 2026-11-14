# Natural Selection / Cell Simulation

Watch as cells interact with their environment and each other, form organisms, and evolve. This project is still in construction.

# General Simulation Information
Cell types:\
  Green Cell(plant cell): Gets sugar when getting sun. Currently this is on the bottom and top of the environment\
  Red Cells: Passively bring sugar into their cell\
  Blue Cells (flagella cell): Has the ability to move around\


<img width="2938" height="1856" alt="Screenshot From 2026-11-13 13-56-45" src="https://github.com/user-attachments/assets/279799a1-43fd-48da-b2a4-bffd5dc12142" />






## How it Works
Cells are born with DNA that make the cell take actions based on different molecules in their environment.\
Then, if they do well enough to reproduce, they pass on their genetics through mitosis along with random genetic mutation.\


<img width="2938" height="1856" alt="Screenshot From 2026-11-12 21-33-49" src="https://github.com/user-attachments/assets/301c7ef6-99a6-4e1a-afc4-0df82d266813" />



## CONTROLS
Use arrow keys to see the different molecule concentrations in the environment. \
  - Red = high concentration\
  - Black = Low Concentration\

Left control paired with left/right mouse click/hold will execute the action chosen in the Interactions window.\

Left click cells to get detailed information inside the Cell Info window.\

left click a specie inside the specie window to see every cell that is part of the selected species.

# Build

Use cmake, put all lib files into build folder\
OpenGL is used with GLFW


