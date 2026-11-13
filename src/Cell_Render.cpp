#include "Cell_Render.hpp"
#include "AdhesionConnection.hpp"



void Cell_Render::notify_add_connection(){
	adhesion_markers.push_back(render_rect_man->new_rect(window->screen_to_UV_pos((Position2D{-2.5, -2.5} + stuck_cells[stuck_cells.size()-1]->get_conn_global_location(dynamic_cast<Cell*>(this)))*sim_ratio, sim_size), window->screen_to_UV_vector(Position2D{5, 5}*sim_ratio, sim_size), 0.0f, 0.0f, 0.0f, 1.0f, .2f));
	
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 0).tx = 0.0;
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 0).ty = 0.0;
								
								
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 1).tx = 1.0;
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 1).ty = 0.0;
								
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 2).tx = 0.0;
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 2).ty = 1.0;
								
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 3).tx = 1.0;
	render_rect_man->get_vertex(adhesion_markers[adhesion_markers.size()-1], 3).ty = 1.0;
	
	
}
void Cell_Render::notify_remove_connection(int i){
	
	
	render_rect_man->destroy_rect(adhesion_markers[i]);
	adhesion_markers.erase(adhesion_markers.begin() + i);
	
	
	
	
}

void Cell_Render::render_step(int selected_species_id){
	
	
	//render_rect_man->set_rect_pos(rect_id, window->screen_to_UV_pos(this->pos - Position2D{size, size}));
	
	Position2D bot_left = window->screen_to_UV_pos((pos - Position2D{size, size}) * sim_ratio, sim_size);
	Position2D top_left = window->screen_to_UV_pos((pos + Position2D{size, size}) * sim_ratio, sim_size);
	
	render_rect_man->get_vertex(rect_id, 0).x = bot_left.x;
	render_rect_man->get_vertex(rect_id, 0).y = bot_left.y;
	
	render_rect_man->get_vertex(rect_id, 1).x = top_left.x;
	render_rect_man->get_vertex(rect_id, 1).y = bot_left.y;
	
	render_rect_man->get_vertex(rect_id, 2).x = bot_left.x;
	render_rect_man->get_vertex(rect_id, 2).y = top_left.y;
	
	render_rect_man->get_vertex(rect_id, 3).x = top_left.x;
	render_rect_man->get_vertex(rect_id, 3).y = top_left.y;
	
	
	float r, g, b;
	switch (cell_type){
		case CellType::NONE:
			r = .10; g = .1; b = .1;
			break;
		case CellType::EATER:
			r = .7; g = .1; b = .1;
			break;
		case CellType::PLANT:
			r = .1; g = .7; b = .1;
			
			break;
		case CellType::FLAGELLA:
			r = .1; g = .7; b = .7;
			
			break;
		
	}
	if (selected_species_id != -1 && selected_species_id == this->specie_parent_index){
		r=1-r;
		g=1-g;
		b=1-b;
	}
	
	render_rect_man->get_vertex(rect_id, 0).r = r;
	render_rect_man->get_vertex(rect_id, 0).g = g;
	render_rect_man->get_vertex(rect_id, 0).b = b;
	
	render_rect_man->get_vertex(rect_id, 1).r = r;
	render_rect_man->get_vertex(rect_id, 1).g = g;
	render_rect_man->get_vertex(rect_id, 1).b = b;
	
	render_rect_man->get_vertex(rect_id, 2).r = r;
	render_rect_man->get_vertex(rect_id, 2).g = g;
	render_rect_man->get_vertex(rect_id, 2).b = b;
	
	render_rect_man->get_vertex(rect_id, 3).r = r;
	render_rect_man->get_vertex(rect_id, 3).g = g;
	render_rect_man->get_vertex(rect_id, 3).b = b;
	
	
	
	render_rect_man->set_rect_pos(angle_notifier_id, window->screen_to_UV_pos((this->pos - Position2D{2.5, 2.5} + Position2D{size * static_cast<float>(cos(angle_rads)), size * -static_cast<float>(sin(angle_rads))}) * sim_ratio, sim_size));
	
	
	
	
	
	for (int i = 0;i < static_cast<int>(adhesion_markers.size()); i++){
		if (stuck_cells[i]->get_is_disconnected()){
			continue;
		}
		render_rect_man->set_rect_pos(adhesion_markers[i], window->screen_to_UV_pos((Position2D{-2.5, -2.5} + stuck_cells[i]->get_conn_global_location(dynamic_cast<Cell*>(this))) * sim_ratio, sim_size));
		
		
		
	}
	
	
}
	
	

Cell_Render::Cell_Render(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc, Render_Rect_Manager<CellVert>* render_rect_man, Window* window) : Cell(env, cell_man, pos, conc){
	
	this->render_rect_man = render_rect_man;
	this->window = window;
	//std::cout<<"s: "<<size<<"\n";
	
	float r, g, b;
	switch (cell_type){
		case CellType::NONE:
			r = .10; g = .1; b = .1;
			break;
		case CellType::EATER:
			r = .7; g = .1; b = .1;
			break;
		case CellType::PLANT:
			r = .1; g = .7; b = .1;
			
			break;
		
	}
	
	sim_size = Position2D{window->height, window->height};
	sim_ratio = Position2D{sim_size.x/env->get_max_pos(),sim_size.y/env->get_max_pos()};
	
	rect_id = render_rect_man->new_rect(window->screen_to_UV_pos((this->pos- Position2D{size, size})*sim_ratio, sim_size), window->screen_to_UV_vector(Position2D{size*2.0f, size*2.0f}*sim_ratio, sim_size), 0.0f, 0.0f, r, g, b);
	
	render_rect_man->get_vertex(rect_id, 0).tx = 0.0;
	render_rect_man->get_vertex(rect_id, 0).ty = 0.0;
	
	render_rect_man->get_vertex(rect_id, 1).tx = 1.0;
	render_rect_man->get_vertex(rect_id, 1).ty = 0.0;
	
	render_rect_man->get_vertex(rect_id, 2).tx = 0.0;
	render_rect_man->get_vertex(rect_id, 2).ty = 1.0;
	
	render_rect_man->get_vertex(rect_id, 3).tx = 1.0;
	render_rect_man->get_vertex(rect_id, 3).ty = 1.0;
	
	
	angle_notifier_id = render_rect_man->new_rect(window->screen_to_UV_pos((this->pos - Position2D{2.5, 2.5} + Position2D{size * static_cast<float>(cos(angle_rads)), size * -static_cast<float>(sin(angle_rads))})*sim_ratio, sim_size), window->screen_to_UV_vector(Position2D{5, 5}*sim_ratio, sim_size), 0.0f, 0.0f, 1.0f, .5f, .2f);
	
	render_rect_man->get_vertex(angle_notifier_id, 0).tx = 0.0;
	render_rect_man->get_vertex(angle_notifier_id, 0).ty = 0.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 1).tx = 1.0;
	render_rect_man->get_vertex(angle_notifier_id, 1).ty = 0.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 2).tx = 0.0;
	render_rect_man->get_vertex(angle_notifier_id, 2).ty = 1.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 3).tx = 1.0;
	render_rect_man->get_vertex(angle_notifier_id, 3).ty = 1.0;
	
	
}
Cell_Render::Cell_Render(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna, int specie_parent_index, Render_Rect_Manager<CellVert>* render_rect_man, Window* window) : Cell(env, cell_man, pos, conc, parent_start_dna, parent_run_dna, specie_parent_index){
	this->render_rect_man = render_rect_man;
	this->window = window;
	//std::cout<<"s: "<<size<<"\n";
	
	float r, g, b;
	switch (cell_type){
		case CellType::NONE:
			r = .10; g = .1; b = .1;
			break;
		case CellType::EATER:
			r = .7; g = .1; b = .1;
			break;
		case CellType::PLANT:
			r = .1; g = .7; b = .1;
			
			break;
		
	}
	
	sim_size = Position2D{window->height, window->height};
	sim_ratio = Position2D{sim_size.x/env->get_max_pos(), sim_size.y/env->get_max_pos()};
	
	rect_id = render_rect_man->new_rect(window->screen_to_UV_pos((this->pos - Position2D{size, size})*sim_ratio, sim_size), window->screen_to_UV_vector(Position2D{size*2.0f, size*2.0f}*sim_ratio, sim_size), 0.0f, 0.0f, r, g, b);
	
	render_rect_man->get_vertex(rect_id, 0).tx = 0.0;
	render_rect_man->get_vertex(rect_id, 0).ty = 0.0;
	
	render_rect_man->get_vertex(rect_id, 1).tx = 1.0;
	render_rect_man->get_vertex(rect_id, 1).ty = 0.0;
	
	render_rect_man->get_vertex(rect_id, 2).tx = 0.0;
	render_rect_man->get_vertex(rect_id, 2).ty = 1.0;
	
	render_rect_man->get_vertex(rect_id, 3).tx = 1.0;
	render_rect_man->get_vertex(rect_id, 3).ty = 1.0;
	
	
	angle_notifier_id = render_rect_man->new_rect(window->screen_to_UV_pos((this->pos - Position2D{2.5, 2.5} + Position2D{size * static_cast<float>(cos(angle_rads)), size * -static_cast<float>(sin(angle_rads))})*sim_ratio, sim_size), window->screen_to_UV_vector(Position2D{5, 5}*sim_ratio, sim_size), 0.0f, 0.0f, 1.0f, .5f, .2f);
	
	render_rect_man->get_vertex(angle_notifier_id, 0).tx = 0.0;
	render_rect_man->get_vertex(angle_notifier_id, 0).ty = 0.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 1).tx = 1.0;
	render_rect_man->get_vertex(angle_notifier_id, 1).ty = 0.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 2).tx = 0.0;
	render_rect_man->get_vertex(angle_notifier_id, 2).ty = 1.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 3).tx = 1.0;
	render_rect_man->get_vertex(angle_notifier_id, 3).ty = 1.0;
	
}

Cell_Render::~Cell_Render(){
	
	for (int i = 0;i < static_cast<int>(adhesion_markers.size()); i++){
		
		render_rect_man->destroy_rect(adhesion_markers[i]);
		
		
		
	}
	render_rect_man->destroy_rect(rect_id);
	render_rect_man->destroy_rect(angle_notifier_id);
	
}

