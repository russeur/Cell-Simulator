#include "Cell.hpp"


void Cell::simulation_step(){
	
	size = inside_concentrations[0];
	
	angle_velocity = .01;
	angle_rads += angle_velocity;
	
	
	if (std::abs(vel.x) > 3 || std::abs(vel.y) > 3){
		
		vel = Position2D{0, 0};
		
	} 
	pos += vel;
	vel = Position2D{vel.x*.99, vel.y*.99};
	env->add_molecule_to_pos(pos, MoleculeType::FOOD, .05);
	
	
	//get touching cells in order for adhesion, doesn't need to be fast because you will really ever be touchign like 4 cells max
	std::vector<Cell*> touching_cells = cell_manager->find_touching_cells(this);
	if (stuck_cells.size() < 3){
		for (int i = 0; i < static_cast<int>(touching_cells.size()); i++){
			if (touching_cells[i]->stuck_cells.size() >= 3) continue; 
			bool already_stuck = false;
			for (int s = 0; s < static_cast<int>(stuck_cells.size()); s++){
				
				if (touching_cells[i] == stuck_cells[s].cell){
					
					already_stuck = true; 
					continue;
				}
				
				
			}
			if (!already_stuck){
				
				Position2D vector_dif = pos - touching_cells[i]->pos;
				
				
				float port_angle = std::atan(-vector_dif.y/vector_dif.x);
				
				if (vector_dif.x >= 0) stuck_cells.push_back(AdhesionConnection{touching_cells[i], M_PI+port_angle - angle_rads, port_angle - touching_cells[i]->angle_rads});
				else stuck_cells.push_back(AdhesionConnection{touching_cells[i], port_angle - angle_rads, M_PI+port_angle - touching_cells[i]->angle_rads});
				
				
				
				
				
			}
			
		}
	}
	
	for (int i = 0; i<stuck_cells.size(); i++){
		
		Position2D host_port_pos = {size*std::cos(angle_rads + stuck_cells[i].host_angle_rads), -size*std::sin(angle_rads + stuck_cells[i].host_angle_rads)};
		host_port_pos += pos;
		
		Position2D client_port_pos = {stuck_cells[i].cell->size*std::cos(stuck_cells[i].cell->angle_rads + stuck_cells[i].client_angle_rads),  -stuck_cells[i].cell->size*std::sin(stuck_cells[i].cell->angle_rads +stuck_cells[i].client_angle_rads)};
		client_port_pos += stuck_cells[i].cell->pos;
		
		Position2D vector_dif = host_port_pos-client_port_pos;
		float dif = sqrt((vector_dif.x*vector_dif.x) + (vector_dif.y*vector_dif.y));
		if (dif < .001) return;
		const float space = 10.0;
		//if (dif < space) continue;
		
		//std::cout<<"d: "<<vector_dif.x<<", "<<vector_dif.y<<"\n";
		const float connection_strength = .05;
		
		//dif  = std::min(dif, space*1.1f);
		
		float force = -connection_strength;
		//if (dif<space) force *= (dif/space);
		
		//std::cout<<"f: "<<force<<" \n";
		
		
		
		
		vel += Position2D{force * (vector_dif.x/dif), force * (vector_dif.y/dif)} - Position2D{};
		
		
		
	}
	
	
}

bool Cell::is_stuck_to_cell(Cell* client_cell){
	
	for (int i = 0;i < stuck_cells.size(); i++){
		
		if (stuck_cells[i].cell == client_cell) return true;
		
	}
	return false;
}

	
Cell::Cell(Environment* env, CellRenderManager*){
	
	this->env = env;
	
}
Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc){
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	
	
	size = conc[0];
	
	
}


