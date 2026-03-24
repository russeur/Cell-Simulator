#include "Cell.hpp"
#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


void Cell::run_plant_type(){
	
	//std::cout<<"start plant/n";
	float mag = .5 * (10.0f - env->sample_pos(pos)[MoleculeType::FOOD]);
	
	inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency*mag;
	
	
	env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, mag*.01);
	
}
void Cell::run_eater_type(){
	//std::cout<<" start eater ";
	std::vector<Cell*> touching_cells = cell_manager->find_touching_cells(this);
	
	for (int i = 0; i < static_cast<int>(touching_cells.size()); i++){
		
		if (touching_cells[i]->cell_type == CellType::PLANT){
			if (true || env->sample_pos(pos)[MoleculeType::OXYGEN] > .1){
				inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency*10*.5;
				touching_cells[i]->inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= plant_cell_efficiency*10;
				env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, -.05*4);
			}
		}
		
	}
	//std::cout<<"finished new ";
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		if (stuck_cells[i]->get_is_disconnected()){
			
			std::cout<<"DISCONNECTION\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
		}
		Cell* other = stuck_cells[i]->get_other_cell(this);
		if (other->cell_type == CellType::PLANT){
			if (true || env->sample_pos(pos)[MoleculeType::OXYGEN] > .5){
				inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency*10*.5;
				other->inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= plant_cell_efficiency*10;
				env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, -.05*4);
			}
		}
	
	}
	//std::cout<<"good eater\n";
	
	inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= plant_cell_efficiency;
	
	//env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, .05);
	
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
	
	cell_type = CellType::NONE;
	start_dna.reset_life();
	start_dna.mutate();
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI) * 10;
	
	pos.x += size;
	
	
	
	
	cell_manager->add_cell(pos + Position2D{-2*size, 0}, inside_concentrations, start_dna);
	
	
}

bool Cell::try_to_split(){
	
	if (inside_concentrations[static_cast<int>(MoleculeType::FOOD)] > 10.0){
		
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
			run_eater_type();
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
	//std::cout<<"start";
	if (inside_concentrations[0] < 1.4){
		is_dead = true;
		cell_manager->delete_cell(this);
		return;
	}
	
	try_to_split();
	
	
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		if (!stuck_cells[i]->get_is_disconnected()){
			
			stuck_cells[i]->apply_force_to_cell(this);
		}else{
			
			stuck_cells.erase(stuck_cells.begin() + i);
			notify_remove_connection(i);
			i--;
		}
		
	}
	
	pos += vel;
	
	angle_rads += angle_vel;
	
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI) * 10;
	
	
	
	if (!start_dna.is_past_childhood()){
		start_dna.simulate_step(env->sample_pos(pos), env->sample_pos(pos + Position2D{size, size}));
		
		if (start_dna.is_past_childhood()){
			//std::cout<<"adult: "<<start_dna.get_determined_cell_type();
			cell_type = start_dna.get_determined_cell_type();
		}
	}
	
	//get touching cells in order for adhesion, doesn't need to be fast because you will really ever be touchign like 4 cells max
	std::vector<Cell*> touching_cells = cell_manager->find_touching_cells(this);
	//std::cout<<"start touching cells";
	for (int i = 0; i < static_cast<int>(touching_cells.size()); i++){
		
		if (!is_stuck_to_cell(touching_cells[i]) && willing_for_connection() && touching_cells[i]->willing_for_connection()){ // if both me and the other are ready for connection, then do it
			
			std::shared_ptr<AdhesionConnection> new_conn(new AdhesionConnection(this, touching_cells[i]));
				
			this->force_add_adhesion_connection(new_conn); // give ownership to the two cells
			touching_cells[i]->force_add_adhesion_connection(new_conn);
			
			cell_manager->add_adhesion_connection(new_conn);
			
		}
		
	}
	
	//std::cout<<" end1\n";
	
	
	
	
	
	
	run_cell_type();
	
	
	
	
	
	
	//std::cout<<"end2\n";
	
	
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
	if (stuck_cells.size() >= 3) return false;
	switch (cell_type){
		case CellType::NONE:
			return false;
			break;
		case CellType::EATER:
			return true;
			break;
		case CellType::PLANT:
			return true;
			break;
		
	}
	return false;
	
}

bool Cell::get_is_dead(){
	return is_dead;
}
	
Cell::Cell(Environment* env, CellRenderManager*) : start_dna(true){
	
	cell_type = CellType::NONE;
	
	this->env = env;
	
}
Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc) : start_dna(true){
	
	cell_type = CellType::NONE;
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI)*10;
	
	
}

Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc, const Genetics::StartingDNA& parent_dna) : start_dna(parent_dna){
	start_dna.mutate();
	cell_type = CellType::NONE;
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI)*10;
}


Cell::~Cell(){
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		stuck_cells[i]->disconnect_connection();
		
	}
}
