#include "MyGui.hpp"

void MyGui::start_frame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
	
}

void MyGui::create_specie_menu(CellRenderManager* cell_manager){
	ImGui::Begin("Specie Info");
    if (ImGui::BeginTable("Specie Populations", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiCond_FirstUseEver)) {
		
		ImGui::TableSetupColumn("Specie Idx");
		ImGui::TableSetupColumn("Population");
		ImGui::TableHeadersRow();
		
		
		std::vector<size_t> idx(cell_manager->all_specie_info.size());
		std::iota(idx.begin(), idx.end(), 0);
		
		std::sort(idx.begin(), idx.end(), [&cell_manager](size_t i1, size_t i2){return cell_manager->all_specie_info[i1].population > cell_manager->all_specie_info[i2].population;});
		
		for (int i = 0; i < idx.size(); i++){
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::PushID(i);
			bool is_selected = (current_row_select == idx[i]);
			std::string label = std::to_string(static_cast<int>(idx[i]));
			if (ImGui::Selectable(label.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
				current_row_select = idx[i];
				std::cout<<current_row_select<<"\n";
			}
			
			
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%i", cell_manager->all_specie_info[idx[i]].population);
			ImGui::PopID();
		}
	
		ImGui::EndTable();
	}
	ImGui::End();
}


void MyGui::create_cell_menu(Environment* env){
    ImGui::Begin("Cell Info", NULL, 0);
    //std::cout<<MyGui::focused_cell.lock().get()<<": x\n";
    if (auto target_cell = focused_cell.lock()){
        
		

		std::string cell_type_string = "Cell Type: ";
		
		cell_type_string += std::to_string(static_cast<int>(target_cell->cell_type));
		
		ImGui::Text(cell_type_string.c_str());


		
		int data[num_of_cell_types*number_of_molecule_types];
		double positions[number_of_molecule_types];
		for (int i = 0; i < number_of_molecule_types; i++){
			positions[i] = i;
		}

		for (int c = 0; c < num_of_cell_types; c++){
			for (int m = 0; m < number_of_molecule_types; m++){
				data[c*num_of_cell_types + m] = ((target_cell->start_dna.get_targets() + c)->all_concentration_targets + m)->concentration;
			}
		}


		static bool isEnvironmentOpen = true;
		if (ImGui::CollapsingHeader("Environment", &isEnvironmentOpen)) 
		{
			if (ImPlot::BeginPlot("Inside Concentrations")) {
			   // 1. Setup the X axis to accept custom text labels
			   ImPlot::SetupAxis(ImAxis_X1, "Molecule Types");
			   // 2. Map the text labels to their respective numeric X coordinates
		   		// Parameters: (Axis, Positions array, Labels array, Number of items, keep_default_ticks)
			   ImPlot::SetupAxisTicks(ImAxis_X1, positions, number_of_molecule_types, mol_name);

			   ImPlot::SetupAxis(ImAxis_Y1, "Concentration Targets");

			   // 3. Draw the bars centered on those same position coordinates
			   // Parameters: (Label, X positions, Y values, Count, Width of bars)
			   ImPlot::PlotBars("Targets", target_cell->inside_concentrations.data(), number_of_molecule_types); 

			   ImPlot::EndPlot();
			}

		}

		static bool isStartingOpen = true;
		if (ImGui::CollapsingHeader("Starting DNA", &isStartingOpen)) 
		{
			// Render the child window only when expanded (or always, depending on your layout/
			for (int c = 0; c < num_of_cell_types; c++){
				ImGui::PushID(c);
				if (ImPlot::BeginPlot("Cell Type")) {
				   // 1. Setup the X axis to accept custom text labels
				   ImPlot::SetupAxis(ImAxis_X1, "Molecule Types");
					   // 2. Map the text labels to their respective numeric X coordinates
			   // Parameters: (Axis, Positions array, Labels array, Number of items, keep_default_ticks)
				   ImPlot::SetupAxisTicks(ImAxis_X1, positions, number_of_molecule_types, mol_name);

				   ImPlot::SetupAxis(ImAxis_Y1, "Concentration Targets");

				   // 3. Draw the bars centered on those same position coordinates
				   // Parameters: (Label, X positions, Y values, Count, Width of bars)
				   ImPlot::PlotBars("Targets", data+(c*num_of_cell_types), number_of_molecule_types); 

				   ImPlot::EndPlot();
				}
				ImGui::PopID();
			}





		}

		static bool isRunningOpen = true;
		if (ImGui::CollapsingHeader("Running DNA", &isRunningOpen)) 
		{
			// Render the child window only when expanded (or always, depending on your layout/
			for (int a = 0; a < target_cell->running_dna.get_number_of_targets(); a++){
				ImGui::PushID(a);

				MoleculeConcentrations outside_conc = env->sample_pos(target_cell->pos + Position2D{target_cell->size, target_cell->size});
				MoleculeConcentrations inside_conc = target_cell->inside_concentrations;
				MoleculeConcentrations diff_conc;
				for (int m = 0; m < number_of_molecule_types; m++){
					diff_conc[m] = outside_conc[m] - inside_conc[m];
				}
				


				if (ImPlot::BeginPlot(action_name[static_cast<int>((target_cell->running_dna.get_targets()+a)->action_type)])) {
				   	// 1. Setup the X axis to accept custom text labels
				   	ImPlot::SetupAxis(ImAxis_X1, "Molecule Types");
				
				   	ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 15);
				
				   	ImPlot::SetupAxis(ImAxis_Y1, "Concentration Ranges");
				
				   	// 3. Draw the bars centered on those same position coordinates
				   	// Parameters: (Label, X positions, Y values, Count, Width of bars)
				   	auto* drawList = ImPlot::GetPlotDrawList();

				   	ImVec2 p1 = ImPlot::PlotToPixels(0, ((target_cell->running_dna.get_targets()+a)->concentration_target.concentration)-((target_cell->running_dna.get_targets()+a)->target_range));
					ImVec2 p2 = ImPlot::PlotToPixels(1, ((target_cell->running_dna.get_targets()+a)->concentration_target.concentration)+((target_cell->running_dna.get_targets()+a)->target_range));
					drawList->AddRectFilled(
    	       		   p1,
    	       		   p2,
    	       		   IM_COL32(70, 130, 255, 255),
    	       		   5.0f
					);

					ImVec2 lp1 = ImPlot::PlotToPixels(0, target_cell->running_dna.get_action_target_actual(a, diff_conc, inside_conc, outside_conc));
					ImVec2 lp2 = ImPlot::PlotToPixels(1, target_cell->running_dna.get_action_target_actual(a, diff_conc, inside_conc, outside_conc));


					drawList->AddLine(lp1, lp2, IM_COL32(255, 255, 0, 255), 2.0f);
					drawList->AddText(
            		    ImVec2(p1.x + 6, p1.y + 5),
            		    IM_COL32(255, 255, 255, 255),
            		    mol_name[static_cast<int>((target_cell->running_dna.get_targets()+a)->concentration_target.molecule_type)]
            		);

					
				   	ImPlot::EndPlot();
				}
				ImGui::PopID();
			}
			

			
			
			
		}
		

	}else{
		focused_cell.reset();
	}
    ImGui::End();
}


void MyGui::create_interaction_menu(){

    ImGui::Begin("Interactions", NULL, 0);

    ImGui::RadioButton("Regular", &current_pointer_action, 0);ImGui::SameLine();
    ImGui::RadioButton("Kill", &current_pointer_action, 1);ImGui::SameLine();
    ImGui::RadioButton("Build", &current_pointer_action, 2);ImGui::SameLine();
    ImGui::RadioButton("Inject", &current_pointer_action, 3);ImGui::SameLine();
    ImGui::RadioButton("Spawn", &current_pointer_action, 4);


    switch (current_pointer_action)
    {
    case 0:
        
        break;
    case 1:
        
        break;
    case 2:
        
        break;
    case 3:
        for (int mol = 0; mol<number_of_molecule_types; mol++){
            ImGui::SliderFloat(mol_name[mol], &inject_concentrations[mol], -1.0f, 1.0f, "%.2f");
        }
        
        
        break;
    case 4:
        
        break;
    default:
        break;
    }



    ImGui::End();

}


