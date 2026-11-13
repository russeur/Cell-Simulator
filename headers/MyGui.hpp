#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <implot.h>

#include <vector>
#include <numeric> 
#include <algorithm>
#include <iostream>
#include "CellManager.hpp"




namespace MyGui
{
    void start_frame();

    inline int current_row_select = -1;
    void create_specie_menu(CellRenderManager* cell_manager);

    inline std::weak_ptr<Cell> focused_cell;
    void create_cell_menu(Environment* env);

    inline int current_pointer_action = 0;
    inline MoleculeConcentrations inject_concentrations;
    void create_interaction_menu();

     
} // namespace MyGui





