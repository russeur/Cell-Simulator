#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


bool CellRenderManager::fix_collision(Cell* cell_a, Cell* cell_b)
{
    Position2D delta = cell_b->pos - cell_a->pos;

    float dist_sq = delta.x * delta.x + delta.y * delta.y;

    if (dist_sq == 0.0f)
        return false;

    float combined_size = cell_a->size + cell_b->size;

    if (dist_sq >= combined_size * combined_size)
        return false;

    float dist = sqrtf(dist_sq);

    float overlap = combined_size - dist;

    cell_a->add_cell_overlap(overlap);
    cell_b->add_cell_overlap(overlap);

    float inv_dist = 1.0f / dist;

    Position2D normal = {
        delta.x * inv_dist,
        delta.y * inv_dist
    };

    // Relative velocity along collision normal
    float vel_on_normal =
        normal.x * (cell_b->vel.x - cell_a->vel.x) +
        normal.y * (cell_b->vel.y - cell_a->vel.y);

    float restitution = 0.5f;

    float j = -(1.0f + restitution) * vel_on_normal;

    j /= (1.0f / cell_a->mass + 1.0f / cell_b->mass);

    // Positional correction
    float correction = overlap * 0.4f;

    cell_a->pos.x -= normal.x * correction;
    cell_a->pos.y -= normal.y * correction;

    cell_b->pos.x += normal.x * correction;
    cell_b->pos.y += normal.y * correction;

    // Collision forces
    cell_a->add_force({
        -j * normal.x,
        -j * normal.y
    });

    cell_b->add_force({
        j * normal.x,
        j * normal.y
    });

    return true;
}

bool CellRenderManager::fix_wall_collisions(Cell* cell_ptr){
	bool hit = false;
	
	if (cell_ptr->pos.x - cell_ptr->size < 0){
		//cell_ptr->vel.x = std::max(cell_ptr->vel.x, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.x *= -1*.5;
		cell_ptr->pos.x = cell_ptr->size; // move away from wall
		hit = true;
	}else if (cell_ptr->pos.x + cell_ptr->size > env->get_max_pos()){
		//cell_ptr->vel.x = std::min(cell_ptr->vel.x, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.x *= -1*.5;
		cell_ptr->pos.x = env->get_max_pos() - cell_ptr->size; // move away from wall
		hit = true;
	}
	
	if (cell_ptr->pos.y - cell_ptr->size < 0){
		//cell_ptr->vel.y = std::max(cell_ptr->vel.y, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.y *= -1*.5;
		cell_ptr->pos.y = cell_ptr->size; // move away from wall
		hit = true;
	}else if (cell_ptr->pos.y + cell_ptr->size > env->get_max_pos()){
		//cell_ptr->vel.y = std::min(cell_ptr->vel.y, 0.0f);//make sure velocity isn't trying to put it in wall
		cell_ptr->vel.y *= -1*.5;
		cell_ptr->pos.y = env->get_max_pos() - cell_ptr->size; // move away from wall
		hit = true;
	}
	
	return hit;
	
}

bool CellRenderManager::fix_env_wall_collisions(Cell* cell_ptr){
	
	int tile_pos = env->get_tile_from_pos(cell_ptr->pos);
	for (int i = tile_pos; i < tile_pos+3; i++){
		fix_env_collision(cell_ptr, i-env->world_dim);
	}
	for (int i = tile_pos; i < tile_pos+3; i++){
		fix_env_collision(cell_ptr, i);
	}
	for (int i = tile_pos; i < tile_pos+3; i++){
		fix_env_collision(cell_ptr, i+env->world_dim);
	}
	return false;
}

bool CellRenderManager::fix_env_collision(Cell* cell_ptr, int tile_pos){
	if (env->concentrations[number_of_molecule_types*env->world_dim*env->world_dim+tile_pos]!=1) return false;
	
	Position2D real_tile_pos = env->get_pos_from_tile(tile_pos);

	Position2D delta = real_tile_pos - cell_ptr->pos;

    float dist_sq = delta.x * delta.x + delta.y * delta.y;

    if (dist_sq == 0.0f)
        return false;

    float combined_size = env->square_size/1.5 + cell_ptr->size;

    if (dist_sq >= combined_size * combined_size) {return false;}
	
	

    float dist = sqrtf(dist_sq);

    float overlap = combined_size - dist;

    
    cell_ptr->add_cell_overlap(overlap);

    float inv_dist = 1.0f / dist;

    Position2D normal = {
        delta.x * inv_dist,
        delta.y * inv_dist
    };

    // Relative velocity along collision normal
    float vel_on_normal =
        normal.x * (cell_ptr->vel.x - 0) +
        normal.y * (cell_ptr->vel.y - 0);

    float restitution = 0.5f;

    float j = -(1.0f + restitution) * vel_on_normal;

    j /= (1/100.0 + 1.0f / cell_ptr->mass);

    // Positional correction
    float correction = -overlap * 0.4f;

    

    cell_ptr->pos.x += normal.x * correction;
    cell_ptr->pos.y += normal.y * correction;

    // Collision forces
    

    cell_ptr->add_force({
        j * normal.x,
        j * normal.y
    });

    return true;


}

void CellRenderManager::find_init_species(){
	
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		
		specie_genes.push_back(std::make_unique<Genetics::genome>(Genetics::genome{cell_list[i]->start_dna, cell_list[i]->running_dna}));
		cell_list[i]->specie_parent_index = i;
		
	}
}
void CellRenderManager::check_for_new_species(Cell* new_cell){
	
	float least_diff = 10.0; // never can go above 1.0
	int index_of_least_diff = -1;
	
	for (int i = 0; i < static_cast<int>(specie_genes.size()); i++){
		
		float current_diff = Genetics::genetic_difference(&(new_cell->running_dna), &(new_cell->start_dna), &(specie_genes[i]->running_dna), &(specie_genes[i]->starting_dna));
		if (current_diff < least_diff){
			least_diff = current_diff;
			index_of_least_diff = i;
		}
		
	}
	
	if (least_diff > .5){// new species
		
		specie_genes.push_back(std::move(std::make_unique<Genetics::genome>(Genetics::genome{new_cell->start_dna, new_cell->running_dna})));
		all_specie_info.push_back(Specie_Info{1, nullptr});
		int new_specie_idx = specie_genes.size()-1;
		if (new_cell->specie_parent_index != -1){
			
			Specie_Node* parent_node = all_specie_info[new_cell->specie_parent_index].tree_node;
			
			
			parent_node->children.push_back(Specie_Node{new_specie_idx, parent_node});
			all_specie_info[new_specie_idx].tree_node = &(*std::next(parent_node->children.begin(), parent_node->children.size()-1));

			
		}else{
			
			head_specie_nodes.push_back(Specie_Node{new_specie_idx, nullptr});

			all_specie_info[new_specie_idx].tree_node = &(*std::next(head_specie_nodes.begin(), head_specie_nodes.size()-1));

		}
		new_cell->specie_parent_index = new_specie_idx;
		
		
		std::cout<<"new species!\n";
		
	}else{
		new_cell->specie_parent_index = index_of_least_diff;
		all_specie_info[index_of_least_diff].population++;
		
	}
	
}


void CellRenderManager::add_cell(Position2D pos, MoleculeConcentrations conc){
	
	cell_list.push_back(std::make_unique<Cell_Render>(env, this, pos, conc, render_rect_man, window));
	
	collision_list.push_back(std::vector<std::weak_ptr<Cell>>());
	
	
}
void CellRenderManager::add_cell(Position2D pos, MoleculeConcentrations conc, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna, int specie_parent_index){
	cell_list.push_back(std::make_unique<Cell_Render>(env, this, pos, conc, parent_start_dna, parent_run_dna, specie_parent_index, render_rect_man, window));
	
	collision_list.push_back(std::vector<std::weak_ptr<Cell>>());
	
}

void CellRenderManager::delete_cell(Cell* cell_ptr){
	/*
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		if (cell_list[i].get() == cell_ptr){
			
			cell_list.erase(cell_list.begin() + i);
			
			collision_list.erase(collision_list.begin() + i);
			
			return;
		}
	}
	*/
	
}
void CellRenderManager::add_adhesion_connection(std::weak_ptr<AdhesionConnection> new_conn){
	
	adhesion_connections.push_back(new_conn);
	
}



std::vector<std::weak_ptr<Cell>> CellRenderManager::find_touching_cells(Cell* client_cell){ //make this based off of indx instead of pointer
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		if (cell_list[i].get() == client_cell){
			
			return collision_list[i];
		}
	}
	
	return std::vector<std::weak_ptr<Cell>>();
}
std::vector<std::weak_ptr<Cell>> CellRenderManager::find_touching_cells(int idx){ //make this based off of indx instead of pointer
	if (idx < static_cast<int>(collision_list.size())) return collision_list[idx];
	
	return std::vector<std::weak_ptr<Cell>>();
}


void CellRenderManager::simulation_step(GLuint m_shaderProg, GLuint BufSize, GLuint m_posBuf[2], int read_buff_idx){
	//std::cout<<"start sim\n";
	
	
	
	
	
	
	
	for (int i = 0; i < static_cast<int>(adhesion_connections.size()); i++){
		if (auto current_conn = adhesion_connections[i].lock()){
			current_conn->simulation_step();
		}else{
			
			adhesion_connections.erase(adhesion_connections.begin() + i);
			i--;
			
		}
	}
	
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		if (cell_list[i]->get_is_dead()){
			
			if (!(all_specie_info[cell_list[i]->specie_parent_index].population == -1)){
				
				all_specie_info[cell_list[i]->specie_parent_index].population--;
			}
			cell_list.erase(cell_list.begin() + i);
			
			collision_list.erase(collision_list.begin() + i);
			i--;
			
			return;
			
		}
	}

	for (int i = 0; i<static_cast<int>(cell_list.size()); i++){
		fix_env_wall_collisions(cell_list[i].get());
	}

	
	const int square_size = 80;
	const int num_squares_x = 1+(Environment::world_dim * Environment::square_size/square_size);

	std::vector<int> spatial_map[num_squares_x*num_squares_x];


	for (int i = 0; i<static_cast<int>(cell_list.size()); i++){
		int x_idx = std::floor(cell_list[i]->pos.x / (float)square_size);
		int y_idx = std::floor(cell_list[i]->pos.y / (float)square_size);

		spatial_map[x_idx + (y_idx * num_squares_x)].push_back(i);
	}


	for (int spatial_i = 0; spatial_i < num_squares_x*num_squares_x; spatial_i++){
		for (int cell_i = 0; cell_i < static_cast<int>(spatial_map[spatial_i].size()); cell_i++){
			Cell* og_cell = cell_list[spatial_map[spatial_i][cell_i]].get();
			
			for (int cell_j = cell_i+1; cell_j < static_cast<int>(spatial_map[spatial_i].size()); cell_j++){
					
				Cell* other_cell = cell_list[spatial_map[spatial_i][cell_j]].get();
				if (fix_collision(og_cell, other_cell) && (!og_cell->is_stuck_to_cell(other_cell))){
			
				// add collision to the memory of collisions this frame
				collision_list[spatial_map[spatial_i][cell_i]].push_back(cell_list[spatial_map[spatial_i][cell_j]]);
				collision_list[spatial_map[spatial_i][cell_j]].push_back(cell_list[spatial_map[spatial_i][cell_i]]);
				fix_wall_collisions(cell_list[spatial_map[spatial_i][cell_i]].get());
				}
			}
			
			
			int adj_spatial_idx[2] = {spatial_i-1, spatial_i-num_squares_x};
			for (int adj_i = 0; adj_i < 2; adj_i++){
				if (adj_spatial_idx[adj_i] < 0 || adj_spatial_idx[adj_i] >= num_squares_x*num_squares_x) continue;
				for (int cell_j = 0; cell_j < static_cast<int>(spatial_map[adj_spatial_idx[adj_i]].size()); cell_j++){
					
					Cell* other_cell = cell_list[spatial_map[adj_spatial_idx[adj_i]][cell_j]].get();
					if (fix_collision(og_cell, other_cell) && (!og_cell->is_stuck_to_cell(other_cell))){
				
					// add collision to the memory of collisions this frame
					collision_list[spatial_map[spatial_i][cell_i]].push_back(cell_list[spatial_map[adj_spatial_idx[adj_i]][cell_j]]);
					collision_list[spatial_map[adj_spatial_idx[adj_i]][cell_j]].push_back(cell_list[spatial_map[spatial_i][cell_i]]);
					fix_wall_collisions(cell_list[spatial_map[spatial_i][cell_i]].get());
					}
				}
			}



		}

	}

	


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_posBuf[1-read_buff_idx]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, number_of_molecule_types*env->world_dim*env->world_dim*sizeof(float), env->concentrations);
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		
		cell_list[i]->simulation_step(i);
		
		fix_wall_collisions(cell_list[i].get());
		
		collision_list[i].clear();
		
		
	}

	/*
	for (int i = 0; i<static_cast<int>(cell_list.size()); i++){
		fix_wall_collisions(cell_list[i].get());
		for (int j = i+1; j < static_cast<int>(cell_list.size()); j++){
			
			if (fix_collision(cell_list[i].get(), cell_list[j].get()) && (!cell_list[i]->is_stuck_to_cell(cell_list[j].get()))){
				
				// add collision to the memory of collisions this frame
				collision_list[i].push_back(cell_list[j]);
				collision_list[j].push_back(cell_list[i]);
				fix_wall_collisions(cell_list[i].get());
				
				
				
				
				
			}
			
		}
		
		
	}*/
	//std::cout<<"end sim\n";
	
}

void CellRenderManager::render_step(int selected_species_id){
	
	for (int i = 0; i < static_cast<int>(cell_list.size()); i++){
		static_cast<Cell_Render*>(cell_list[i].get())->render_step(selected_species_id);
	}
	
}

CellRenderManager::CellRenderManager(Environment* env, Render_Rect_Manager<CellVert>* render_rect_man, Window* window){
	
	this->env = env;
	this->render_rect_man = render_rect_man;
	this->window = window;
}