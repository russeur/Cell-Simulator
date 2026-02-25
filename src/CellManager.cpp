#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


bool CellRenderManager::fix_collision(Cell* cell_a, Cell* cell_b){
	
	
	Position2D dist_vector = cell_a->pos - cell_b->pos;
	float dist = sqrt((dist_vector.x*dist_vector.x) + (dist_vector.y*dist_vector.y));
	if (dist == 0) return false;
	float overlap = cell_a->size + cell_b->size - dist;
	if (overlap <= 0) return false;
	
	//we know they are colliding now!!
	
	
	
	if (cell_a->vel == Position2D{0.0, 0.0} && cell_b->vel == Position2D{0.0, 0.0}){
		
		
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		//cell_a->pos -= Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		//cell_b->pos += Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		
		
		
	}else if (false && cell_a->is_stuck_to_cell(cell_b)){
		
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f *.8f, sin_cos.y*overlap*.5f *.8f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f *.8f, sin_cos.y*overlap*.5f *.8f};
		
		
		
		
		
	}else if (true){
		
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f *.1f, sin_cos.y*overlap*.5f *.1f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f *.1f, sin_cos.y*overlap*.5f *.1f};
		//Position2D sum_vel = cell_a->vel + cell_b->vel;
		
		Position2D vel_dif = cell_a->vel - cell_b->vel;
		
		float velAlongNormal = vel_dif.x*sin_cos.x + vel_dif.y*sin_cos.y;
		
		//if (velAlongNormal > 0){
		//	std::cout<<"bad col\n";
		//	return true;
		//}
		
		float restitution = .2f;  // 1.0 = perfect bounce, 0 = no bounce

		float j = -(1 + restitution) * velAlongNormal;
		j /= (1 / cell_a->mass + 1 / cell_b->mass); // cell_a->szie = mass
		
		
		cell_a->add_force(sin_cos * Position2D{j, j});
		cell_b->add_force(sin_cos * Position2D{-j, -j});
		
		
		
		
		//cell_a->vel = sum_vel * Position2D{.5f, .5f};
		//cell_b->vel = sum_vel  * Position2D{.5f, .5f};
		
	}else{
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		
		Position2D relative_vel = cell_a->vel - cell_b->vel;
		float vel_dist = sqrt((relative_vel.x*relative_vel.x) + (relative_vel.y*relative_vel.y));
		
		const float bounce_decay = 1.0;
		
		cell_a->vel += Position2D{sin_cos.x*vel_dist*.5, sin_cos.y*vel_dist*.5};
		cell_b->vel -= Position2D{sin_cos.x*vel_dist*.5, sin_cos.y*vel_dist*.5};
		
		cell_a->vel = cell_a->vel * Position2D{bounce_decay, bounce_decay};
		cell_b->vel = cell_b->vel * Position2D{bounce_decay, bounce_decay};
		
		
		
	}
	
	
	return true;
}
bool CellRenderManager::fix_wall_collisions(Cell* cell_ptr){
	bool hit = false;
	
	if (cell_ptr->pos.x - cell_ptr->size < 0){
		//cell_ptr->vel.x = std::max(cell_ptr->vel.x, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.x *= -1;
		cell_ptr->pos.x = cell_ptr->size; // move away from wall
		hit = true;
	}else if (cell_ptr->pos.x + cell_ptr->size > env->get_max_pos()){
		//cell_ptr->vel.x = std::min(cell_ptr->vel.x, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.x *= -1;
		cell_ptr->pos.x = env->get_max_pos() - cell_ptr->size; // move away from wall
		hit = true;
	}
	
	if (cell_ptr->pos.y - cell_ptr->size < 0){
		//cell_ptr->vel.y = std::max(cell_ptr->vel.y, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.y *= -1;
		cell_ptr->pos.y = cell_ptr->size; // move away from wall
		hit = true;
	}else if (cell_ptr->pos.y + cell_ptr->size > env->get_max_pos()){
		//cell_ptr->vel.y = std::min(cell_ptr->vel.y, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.y *= -1;
		cell_ptr->pos.y = env->get_max_pos() - cell_ptr->size; // move away from wall
		hit = true;
	}
	
	return hit;
	
}

void CellRenderManager::add_cell(Position2D pos, MoleculeConcentrations conc){
	
	cell_list.push_back(std::make_unique<Cell_Render>(env, this, pos, conc, render_rect_man, window));
	
	collision_list.push_back(std::vector<Cell*>());
	
	
}
void CellRenderManager::delete_cell(Cell* cell_ptr){
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		if (cell_list[i].get() == cell_ptr){
			cell_list.erase(cell_list.begin() + i);
			collision_list.erase(collision_list.begin() + i);
			return;
		}
	}
	
	
}




std::vector<Cell*> CellRenderManager::find_touching_cells(Cell* client_cell){
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		if (cell_list[i].get() == client_cell){
			
			return collision_list[i];
		}
	}
	
	return std::vector<Cell*>();
}


void CellRenderManager::simulation_step(){
	
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		cell_list[i]->simulation_step();
		
		bool x = fix_wall_collisions(cell_list[i].get());
		
		collision_list[i].clear();
		
	}
	
	for (int i = 0; i < static_cast<int>(adhesion_connections.size()); i++){
		if (auto current_conn = adhesion_connections[i].lock()){
			current_conn->simulation_step();
		}else{
			adhesion_connections.erase(adhesion_connections.begin() + i);
		}
	}
	
	
	for (int i = 0; i<static_cast<int>(cell_list.size()); i++){
		for (int j = i+1; j < static_cast<int>(cell_list.size()); j++){
			
			if (fix_collision(cell_list[i].get(), cell_list[j].get()) && (!cell_list[i]->is_stuck_to_cell(cell_list[j].get()))){
				
				// add collision to the memory of collisions this frame
				collision_list[i].push_back(cell_list[j].get());
				collision_list[j].push_back(cell_list[i].get());
				
				std::shared_ptr<AdhesionConnection> new_conn(new AdhesionConnection(cell_list[i].get(), cell_list[j].get()));
				
				cell_list[i]->force_add_adhesion_connection(new_conn); // give ownership to the two cells
				cell_list[j]->force_add_adhesion_connection(new_conn);
				
				adhesion_connections.push_back(new_conn);
				
				std::cout<<"add collision\n";
				
				
			}
			
		}
		
		
	}
	
	
}

void CellRenderManager::render_step(){
	
	for (int i = 0; i < cell_list.size(); i++){
		static_cast<Cell_Render*>(cell_list[i].get())->render_step();
	}
	
}

CellRenderManager::CellRenderManager(Environment* env, Render_Rect_Manager<CellVert>* render_rect_man, Window* window){
	
	this->env = env;
	this->render_rect_man = render_rect_man;
	this->window = window;
}