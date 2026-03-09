#include "Cell_Render.hpp"
#include "AdhesionConnection.hpp"



void Cell_Render::notify_add_connection(){
	adhesion_markers.push_back(render_rect_man->new_rect(window->screen_to_UV_pos(Position2D{-2.5, -2.5} + stuck_cells[stuck_cells.size()-1]->get_conn_global_location(dynamic_cast<Cell*>(this))), window->screen_to_UV_vector(Position2D{5, 5}), 0.0f, 0.0f, 0.0f, 1.0f, .2f));
	
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

void Cell_Render::render_step(){
	
	
	//render_rect_man->set_rect_pos(rect_id, window->screen_to_UV_pos(this->pos - Position2D{size, size}));
	
	Position2D bot_left = window->screen_to_UV_pos(pos - Position2D{size, size});
	Position2D top_left = window->screen_to_UV_pos(pos + Position2D{size, size});
	
	render_rect_man->get_vertex(rect_id, 0).x = bot_left.x;
	render_rect_man->get_vertex(rect_id, 0).y = bot_left.y;
	
	render_rect_man->get_vertex(rect_id, 1).x = top_left.x;
	render_rect_man->get_vertex(rect_id, 1).y = bot_left.y;
	
	render_rect_man->get_vertex(rect_id, 2).x = bot_left.x;
	render_rect_man->get_vertex(rect_id, 2).y = top_left.y;
	
	render_rect_man->get_vertex(rect_id, 3).x = top_left.x;
	render_rect_man->get_vertex(rect_id, 3).y = top_left.y;
	
	render_rect_man->set_rect_pos(angle_notifier_id, window->screen_to_UV_pos(this->pos - Position2D{2.5, 2.5} + Position2D{size * static_cast<float>(cos(angle_rads)), size * -static_cast<int>(sin(angle_rads))}));
	
	
	
	
	
	for (int i = 0;i < static_cast<int>(adhesion_markers.size()); i++){
		
		render_rect_man->set_rect_pos(adhesion_markers[i], window->screen_to_UV_pos(Position2D{-2.5, -2.5} + stuck_cells[i]->get_conn_global_location(dynamic_cast<Cell*>(this))));
		
		
		
	}
	
	
}
	
	

Cell_Render::Cell_Render(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc, Render_Rect_Manager<CellVert>* render_rect_man, Window* window) : Cell(env, cell_man, pos, conc){
	
	this->render_rect_man = render_rect_man;
	this->window = window;
	std::cout<<"s: "<<size<<"\n";
	rect_id = render_rect_man->new_rect(window->screen_to_UV_pos(this->pos - Position2D{size, size}), window->screen_to_UV_vector(Position2D{size*2.0f, size*2.0f}), 0.0f, 0.0f, .5f, .5f, .2f);
	
	render_rect_man->get_vertex(rect_id, 0).tx = 0.0;
	render_rect_man->get_vertex(rect_id, 0).ty = 0.0;
	
	render_rect_man->get_vertex(rect_id, 1).tx = 1.0;
	render_rect_man->get_vertex(rect_id, 1).ty = 0.0;
	
	render_rect_man->get_vertex(rect_id, 2).tx = 0.0;
	render_rect_man->get_vertex(rect_id, 2).ty = 1.0;
	
	render_rect_man->get_vertex(rect_id, 3).tx = 1.0;
	render_rect_man->get_vertex(rect_id, 3).ty = 1.0;
	
	
	angle_notifier_id = render_rect_man->new_rect(window->screen_to_UV_pos(this->pos - Position2D{2.5, 2.5} + Position2D{size * static_cast<float>(cos(angle_rads)), size * -static_cast<float>(sin(angle_rads))}), window->screen_to_UV_vector(Position2D{5, 5}), 0.0f, 0.0f, 1.0f, .5f, .2f);
	
	render_rect_man->get_vertex(angle_notifier_id, 0).tx = 0.0;
	render_rect_man->get_vertex(angle_notifier_id, 0).ty = 0.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 1).tx = 1.0;
	render_rect_man->get_vertex(angle_notifier_id, 1).ty = 0.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 2).tx = 0.0;
	render_rect_man->get_vertex(angle_notifier_id, 2).ty = 1.0;
								
	render_rect_man->get_vertex(angle_notifier_id, 3).tx = 1.0;
	render_rect_man->get_vertex(angle_notifier_id, 3).ty = 1.0;
	
	
}


