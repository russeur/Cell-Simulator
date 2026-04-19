#include "Cell.hpp"
#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


void Cell::run_plant_type(){
	
	//std::cout<<"start plant/n";
	float mag = .5 * (10.0f - env->sample_pos(pos)[static_cast<int>(MoleculeType::FOOD)]);
	
	inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency*mag*cell_effiency;
	
	inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += mag*.01;
	//env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, mag*.01);
	
}
void Cell::run_eater_type(){
	//std::cout<<" start eater ";
	std::vector<Cell*> touching_cells = cell_manager->find_touching_cells(this);
	//std::cout<<inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)]<<"\n";
	for (int i = 0; i < static_cast<int>(touching_cells.size()); i++){
		
		if (touching_cells[i]->cell_type == CellType::PLANT){
			if (true || env->sample_pos(pos)[static_cast<int>(MoleculeType::OXYGEN)] > .1){
				inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency*10*.5*cell_effiency;
				touching_cells[i]->inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= plant_cell_efficiency*10*cell_effiency;
				env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, -.05*4*cell_effiency);
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
			if (true || env->sample_pos(pos)[static_cast<int>(MoleculeType::OXYGEN)] > .4){
				inside_concentrations[static_cast<int>(MoleculeType::FOOD)] += plant_cell_efficiency*10*.5*cell_effiency;
				other->inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= plant_cell_efficiency*10*cell_effiency;
				env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, -.05*4*cell_effiency);
			}
		}
	
	}
	
	
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
	
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI) * 10;
	
	pos.x += size;
	
	
	
	
	cell_manager->add_cell(pos + Position2D{-2*size, 0}, inside_concentrations, start_dna, running_dna);
	
	start_dna.reset_life();
	start_dna.mutate();
	
	running_dna.mutate();
	
	
}

void Cell::run_oxy_to_acid(ActionLocation destination){
	//std::cout<<"do x_to_a\n";
	const float strength = .01;
	if (inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] < strength) return;
	inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] -= strength;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::ACID)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::ACID, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			inside_concentrations[static_cast<int>(MoleculeType::ACID)] += strength;
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
	
}
void Cell::run_food_to_oxy(ActionLocation destination){
	//std::cout<<"do f_to_o\n";
	const float strength = .01;
	switch (destination){
		case (ActionLocation::INSIDE):
			inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= strength;
			inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			env->add_molecule_to_pos(pos, MoleculeType::FOOD, -strength);
			env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, strength);
			break;
		case (ActionLocation::ATTACHED):
			inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= strength;
			inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		case (ActionLocation::NONE):
			//inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= strength;
			//inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
	
	
	
}

void Cell::do_all_dna_actions(){
	
	running_dna.simulate_step(env->sample_pos(pos), env->sample_pos(pos + Position2D{size, size}));
	
	for (int i = 0; i < running_dna.get_number_of_recommended_actions(); i++){
		switch (running_dna.get_recommended_action(i)){
			case ActionType::OXY_TO_ACID:
				run_oxy_to_acid(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::FOOD_TO_OXY:
				run_food_to_oxy(running_dna.get_recommended_action_location(i));
				break;
			default:
				std::cout<<"Action type "<<static_cast<int>(running_dna.get_recommended_action(i))<<" is unknown/n";
			
		}
	}
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
		default:
			std::cout<<"CellType of value '"<<static_cast<int>(cell_type)<<"' is not an option\n";
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
		
		env->add_molecules_to_pos(pos, inside_concentrations);
		
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
	
	
	
	cell_effiency -= abs(inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)])/200000.0;
	//std::cout<<"cell_effiency: "<<cell_effiency<<"\n";
	
	if (cell_effiency < 0.0) cell_effiency = 0.0;
	
	
	run_cell_type();
	do_all_dna_actions();
	
	
	
	
	
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
	
Cell::Cell(Environment* env, CellRenderManager*) : start_dna(true), running_dna(true){
	
	cell_type = CellType::NONE;
	
	this->env = env;
	
}
Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc) : start_dna(true), running_dna(true){
	
	cell_type = CellType::NONE;
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	
	
	mass = inside_concentrations[0];
	size = sqrt(mass/M_PI)*10;
	
	
}

Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna) : start_dna(parent_start_dna), running_dna(parent_run_dna){
	start_dna.mutate();
	running_dna.mutate();
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
