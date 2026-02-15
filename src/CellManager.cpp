#include "CellManager.hpp"
#include "Cell_Render.hpp"


bool CellRenderManager::fix_collision(Cell* cell_a, Cell* cell_b){
	
	Position2D dist_vector = cell_a->pos - cell_b->pos;
	float dist = sqrt((dist_vector.x*dist_vector.x) + (dist_vector.y*dist_vector.y));
	float overlap = cell_a->size + cell_b->size - dist;
	if (overlap <= 0) return false;
	
	//we know they are colliding now!!
	
	//TODO fix the collision now that you know they collided
	
	if (cell_a->vel == Position2D{0.0, 0.0} && cell_b->vel == Position2D{0.0, 0.0}){
		
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		
		
		
	}else if (cell_a->is_stuck_to_cell(cell_b)){
		
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		
		
		/*
		Position2D sum_vel = cell_a->vel + cell_b->vel;
		
		cell_a->vel = sum_vel * Position2D{.5f, .5f};
		cell_b->vel = sum_vel  * Position2D{.5f, .5f};
		*/
		
		
	}else if (true){
		Position2D sin_cos = {dist_vector.x/dist, dist_vector.y/dist}; // now I have a normal vector a
		
		cell_a->pos += Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		cell_b->pos -= Position2D{sin_cos.x*overlap*.5f, sin_cos.y*overlap*.5f};
		
		Position2D sum_vel = cell_a->vel + cell_b->vel;
		
		cell_a->vel = sum_vel * Position2D{.5f, .5f};
		cell_b->vel = sum_vel  * Position2D{.5f, .5f};
		
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

void CellRenderManager::add_cell(std::unique_ptr<Cell_Render> cell_ptr){
	
	cell_list.push_back(static_cast<std::unique_ptr<Cell>>(std::move(cell_ptr)));
	
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
		if (x)std::cout<<"hit wall\n";
		collision_list[i].clear();
		
	}
	
	
	for (int i = 0; i<static_cast<int>(cell_list.size()); i++){
		for (int j = i+1; j < static_cast<int>(cell_list.size()); j++){
			
			if (fix_collision(cell_list[i].get(), cell_list[j].get())){
				// add collision to the memory of collisions this frame
				collision_list[i].push_back(cell_list[j].get());
				collision_list[j].push_back(cell_list[i].get());
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

CellRenderManager::CellRenderManager(Environment* env){
	std::cout<<"hi\n";
	this->env = env;
}