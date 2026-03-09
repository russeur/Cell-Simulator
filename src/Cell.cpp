#include "Cell.hpp"
#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


void Cell::run_plant_type(){
	
	inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency;
	
	env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, .05);
	
}

void Cell::split(){
	
	inside_concentrations[0] /=2.0;
	inside_concentrations[1] /=2.0;
	inside_concentrations[2] /=2.0;
	inside_concentrations[3] /=2.0;
	inside_concentrations[4] /=2.0;
	
	
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		stuck_cells[i]->disconnect_connection();
		notify_remove_connection(0);
	}
	stuck_cells.clear();
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI) * 10;
	
	pos.x += size;
	
	
	
	
	cell_manager->add_cell(pos + Position2D{-2*size, 0}, inside_concentrations);
	
	
}

bool Cell::try_to_split(){
	
	if (inside_concentrations[static_cast<int>(MoleculeType::FOOD)] > 70.0){
		
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
		if (!stuck_cells[i]->get_is_disconnected()){
			
			stuck_cells[i]->apply_force_to_cell(this);
		}else{
			
			stuck_cells.erase(stuck_cells.begin() + i);
			notify_remove_connection(i);
		}
		
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
	
	
	
	//get touching cells in order for adhesion, doesn't need to be fast because you will really ever be touchign like 4 cells max
	std::vector<Cell*> touching_cells = cell_manager->find_touching_cells(this);
	
	for (int i = 0; i < static_cast<int>(touching_cells.size()); i++){
		
		if (!is_stuck_to_cell(touching_cells[i]) && willing_for_connection() && touching_cells[i]->willing_for_connection()){ // if both me and the other are ready for connection, then do it
			
			std::shared_ptr<AdhesionConnection> new_conn(new AdhesionConnection(this, touching_cells[i]));
				
			this->force_add_adhesion_connection(new_conn); // give ownership to the two cells
			touching_cells[i]->force_add_adhesion_connection(new_conn);
			
			cell_manager->add_adhesion_connection(new_conn);
			
		}
		
	}
	
	
	
	
	
	
}

bool Cell::is_stuck_to_cell(Cell* client_cell){
	
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		if (stuck_cells[i]->c1 == client_cell || stuck_cells[i]->c2 == client_cell) return true;
		
	}
	return false;
}

void Cell::force_add_adhesion_connection(std::shared_ptr<AdhesionConnection> new_conn){
	
	stuck_cells.push_back(std::move(new_conn));
	notify_add_connection();
}

bool Cell::willing_for_connection(){
	
	if (stuck_cells.size() < 3){
		return true;
	}
	else return false;
	
}
	
Cell::Cell(Environment* env, CellRenderManager*){
	
	cell_type = CellType::PLANT;
	
	this->env = env;
	
}
Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc){
	
	cell_type = CellType::PLANT;
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI)*10;
	
	
}


