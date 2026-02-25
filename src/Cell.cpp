#include "Cell.hpp"
#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


void Cell::run_plant_type(){
	
	inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += .1;
	
	env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, .05);
	
}

void Cell::split(){
	std::cout<<"split!\n";
	inside_concentrations[0] /=2.0;
	inside_concentrations[1] /=2.0;
	inside_concentrations[2] /=2.0;
	inside_concentrations[3] /=2.0;
	inside_concentrations[4] /=2.0;
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI) * 10;
	
	pos.x += size;
	
	
	
	
	cell_manager->add_cell(pos + Position2D{-2*size, 0}, inside_concentrations);
	
	
}

bool Cell::try_to_split(){
	std::cout<<"food: "<<inside_concentrations[static_cast<int>(MoleculeType::FOOD)]<<"\n";
	if (inside_concentrations[static_cast<int>(MoleculeType::FOOD)] > 200.0){
		
		split();
		return true;
	}
	return false;
}

void Cell::run_cell_type(){
	
	switch (cell_type){
		case CellType::NONE:
			break;
		case CellType::EATER:
			break;
		case CellType::PLANT:
			run_plant_type();
			break;
		
	}
	
}

void Cell::add_force(Position2D force){
	
	vel += Position2D{force.x / mass, force.y/mass};
	
}
void Cell::add_torque(float torque){
	angle_vel += torque / (.5 * mass * size*size);
}


void Cell::simulation_step(){
	
	try_to_split();
	
	run_cell_type();
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		stuck_cells[i]->apply_force_to_cell(this);
		
	}
	
	pos += vel;
	
	angle_rads += angle_vel;
	
	env->add_molecule_to_pos(pos, MoleculeType::FOOD, .05);
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI) * 10;
	
	
	
	//cell_group->add_force(pos+Position2D{size, 0}, Position2D{-.005, .001});
	
	//angle_velocity = .01;
	//angle_rads += angle_velocity;
	
	
	
	//pos += vel;
	//vel = Position2D{vel.x*.99, vel.y*.99};
	
	
	/*
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
				/*
				Position2D vector_dif = pos - touching_cells[i]->pos;
				
				
				float port_angle = std::atan(-vector_dif.y/vector_dif.x);
				
				if (vector_dif.x >= 0) stuck_cells.push_back(AdhesionConnection{touching_cells[i], M_PI+port_angle - angle_rads, port_angle - touching_cells[i]->angle_rads});
				else stuck_cells.push_back(AdhesionConnection{touching_cells[i], port_angle - angle_rads, M_PI+port_angle - touching_cells[i]->angle_rads});
				
				
				
				
				
			}
			
		}
	}
	*/
	
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		
		
	}
	
	
}

bool Cell::is_stuck_to_cell(Cell* client_cell){
	
	for (int i = 0;i < stuck_cells.size(); i++){
		
		if (stuck_cells[i]->c1 == client_cell || stuck_cells[i]->c2 == client_cell) return true;
		
	}
	return false;
}

void Cell::force_add_adhesion_connection(std::shared_ptr<AdhesionConnection> new_conn){
	
	stuck_cells.push_back(std::move(new_conn));
	
}

	
Cell::Cell(Environment* env, CellRenderManager*){
	
	cell_type = CellType::PLANT;
	
	this->env = env;
	
}
Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc){
	std::cout<<"new cell\n";
	cell_type = CellType::PLANT;
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI)*10;
	
	
}


