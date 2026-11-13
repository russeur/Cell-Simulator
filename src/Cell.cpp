#include "Cell.hpp"
#include "CellManager.hpp"
#include "AdhesionConnection.hpp"


void Cell::run_plant_type(){
	
	
	inside_concentrations[static_cast<int>(MoleculeType::ATP)] -= env->sample_pos(pos)[static_cast<int>(MoleculeType::PLANTAESE)];
	env->add_molecule_to_pos(pos, MoleculeType::PLANTAESE, -env->sample_pos(pos)[static_cast<int>(MoleculeType::PLANTAESE)]);

	if (pos.y > env->square_size*2 && pos.y <  env->square_size*(env->world_dim-2)) return;
	//std::cout<<"start plant/n";
	if (inside_concentrations[static_cast<int>(MoleculeType::CO2)] <= plant_cell_efficiency*10*cell_effiency) return;
	float mag = 5 * env->sample_pos(pos)[static_cast<int>(MoleculeType::CO2)] * (1.0-(percent_covered));
	//std::cout<<percent_covered<<"pcov, "<< mag<<"mag\n";
	
	if (mag <= 0) return;
	inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] += plant_cell_efficiency*mag*cell_effiency;
	
	
	
	inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += plant_cell_efficiency*mag*cell_effiency*.1;
	inside_concentrations[static_cast<int>(MoleculeType::CO2)] -= plant_cell_efficiency*mag*cell_effiency*.1;
	//env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, mag*.01);
	
}

void Cell::run_eater_type(){
	if (env->sample_pos(pos)[static_cast<int>(MoleculeType::SUGAR)] > 0){
		
	
		env->add_molecule_to_pos(pos, MoleculeType::SUGAR, -plant_cell_efficiency*10);
		inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] += plant_cell_efficiency*10;
	}
	
	if (env->sample_pos(pos)[static_cast<int>(MoleculeType::ATP)] > 0){
		env->add_molecule_to_pos(pos, MoleculeType::ATP, -plant_cell_efficiency*10);
		inside_concentrations[static_cast<int>(MoleculeType::ATP)] += plant_cell_efficiency*10 * .9;
	}
	
}
void Cell::run_flagella_type(){
	
	
	
}

void Cell::split(){
	
	if (inside_concentrations[1] - Genetics::get_genetic_length(&this->running_dna, &this->start_dna) * .15 < 5.0*2.0)return;
	inside_concentrations[1] -= Genetics::get_genetic_length(&this->running_dna, &this->start_dna) * .15;
	
	for (int i = 0; i< number_of_molecule_types; i++){
		inside_concentrations[i] /=2.0;
	}
	
	
	
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		stuck_cells[i]->disconnect_connection();
		notify_remove_connection(0);
	}
	stuck_cells.clear();
	
	cell_type = CellType::NONE;
	
	
	
	mass = inside_concentrations[static_cast<int>(MoleculeType::ATP)];
	size = sqrt(mass/M_PI) * 10;
	
	pos.x += size;
	
	
	
	
	
	cell_manager->add_cell(pos + Position2D{-2*size, 0}, inside_concentrations, start_dna, running_dna, specie_parent_index);
	
	start_dna.reset_life();
	start_dna.mutate();
	
	running_dna.mutate();
	
	cell_manager->all_specie_info[specie_parent_index].population--;
	cell_manager->check_for_new_species(this);
	
}

void Cell::run_oxy_to_acid(ActionLocation destination){
	//std::cout<<"do x_to_a\n";
	
	return;
	const float strength = .000001;
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
void Cell::split_action(ActionLocation destination){
	//std::cout<<"do f_to_o\n";
	
	split();
	return;
	/*
	if (inside_concentrations[static_cast<int>(MoleculeType::FOOD)] <= 0.0) return;
	const float strength = .01;
	inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= strength;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		case (ActionLocation::NONE):
			//inside_concentrations[static_cast<int>(MoleculeType::FOOD)] -= strength;
			//inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
	*/
	
	
}
void Cell::run_food_to_food(ActionLocation destination){
	if (inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] <= 0.0) return;
	const float strength = .001;
	inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] -= strength;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::SUGAR, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			for (int i = 0; i < stuck_cells.size(); i++){
				
				stuck_cells[i]->get_other_cell(this)->inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] += strength / stuck_cells.size();
			}
			
			
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
}
void Cell::run_oxy_to_oxy(ActionLocation destination){
	if (inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] <= 0.0) return;
	const float strength = .001;
	inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] -= strength;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			for (int i = 0; i < stuck_cells.size(); i++){
				
				stuck_cells[i]->get_other_cell(this)->inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength / stuck_cells.size();
			}
			
			
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
}
void Cell::run_outoxy_to_oxy(ActionLocation destination){
	if (env->sample_pos(pos)[static_cast<int>(MoleculeType::OXYGEN)] <= 0.0) return;
	const float strength = .01;
	env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, -strength);
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::OXYGEN, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			for (int i = 0; i < stuck_cells.size(); i++){
				
				stuck_cells[i]->get_other_cell(this)->inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] += strength / stuck_cells.size();
			}
			
			
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
}
void Cell::run_co2_to_co2(ActionLocation destination){
	if (inside_concentrations[static_cast<int>(MoleculeType::CO2)] <= 0.0) return;
	const float strength = .001;
	inside_concentrations[static_cast<int>(MoleculeType::CO2)] -= strength;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::CO2)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::CO2, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			for (int i = 0; i < stuck_cells.size(); i++){
				
				stuck_cells[i]->get_other_cell(this)->inside_concentrations[static_cast<int>(MoleculeType::CO2)] += strength / stuck_cells.size();
			}
			
			
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
}
void Cell::run_outco2_to_co2(ActionLocation destination){
	if (env->sample_pos(pos)[static_cast<int>(MoleculeType::CO2)] <= 0.0) return;
	const float strength = .001;
	env->add_molecule_to_pos(pos, MoleculeType::CO2, -strength);
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::CO2)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::CO2, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			for (int i = 0; i < stuck_cells.size(); i++){
				
				stuck_cells[i]->get_other_cell(this)->inside_concentrations[static_cast<int>(MoleculeType::CO2)] += strength / stuck_cells.size();
			}
			
			
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
}

void Cell::run_atp_to_flagella(ActionLocation destination){
	if (cell_type == CellType::FLAGELLA && inside_concentrations[static_cast<int>(MoleculeType::ATP)] > 0.0){
		inside_concentrations[static_cast<int>(MoleculeType::ATP)] -= .0001;
		this->add_force(Position2D{std::cos(angle_rads)*.01, std::sin(angle_rads)*.01});
	}
	
}
void Cell::run_make_plantaese(ActionLocation destination){
	const float strength = .0001;
	if (inside_concentrations[static_cast<int>(MoleculeType::ATP)] < strength) return;
	inside_concentrations[static_cast<int>(MoleculeType::ATP)] -= strength;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::PLANTAESE)] += strength;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::PLANTAESE, strength);
			break;
		case (ActionLocation::ATTACHED):
			
			inside_concentrations[static_cast<int>(MoleculeType::PLANTAESE)] += strength;
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}
}
void Cell::run_sugar_to_atp(ActionLocation destination){
	const float strength = .1;
	if (inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] < strength*.1*10) return;
	if (inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] < strength) return;
	inside_concentrations[static_cast<int>(MoleculeType::SUGAR)] -= strength;
	
	inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)] -= strength*.1;
	inside_concentrations[static_cast<int>(MoleculeType::CO2)] += strength*.1*.8;
	
	switch (destination){
		case (ActionLocation::INSIDE):
			
			inside_concentrations[static_cast<int>(MoleculeType::ATP)] += strength*.9;
			break;
		case (ActionLocation::OUTSIDE):
			
			env->add_molecule_to_pos(pos, MoleculeType::ATP, strength*.9);
			break;
		case (ActionLocation::ATTACHED):
			
			inside_concentrations[static_cast<int>(MoleculeType::ATP)] += strength*.9;
			break;
		case (ActionLocation::NONE):
			
			break;
		default:
			std::cout<<"ActionLocation of value '"<<static_cast<int>(destination)<<"' is not an option\n";
	}

}
void Cell::run_adhere(ActionLocation destination){
	wants_adhere = true;
}

void Cell::do_all_dna_actions(){
	
	running_dna.simulate_step(inside_concentrations, env->sample_pos(pos + Position2D{size, size}));
	
	
	for (int i = 0; i < running_dna.get_number_of_recommended_actions(); i++){
		switch (running_dna.get_recommended_action(i)){
			case ActionType::OXY_TO_ACID:
				run_oxy_to_acid(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::SPLIT:
				split_action(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::FOOD_TO_FOOD:
				run_food_to_food(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::OXY_TO_OXY:
				run_oxy_to_oxy(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::OUTOXY_TO_OXY:
				run_outoxy_to_oxy(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::CO2_TO_CO2:
				run_co2_to_co2(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::OUTCO2_TO_CO2:
				run_outco2_to_co2(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::ATP_TO_FLAGELLA:
				run_atp_to_flagella(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::MAKE_PLANTAESE:
				run_make_plantaese(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::SUGAR_TO_ATP:
				run_sugar_to_atp(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::RUN_ADHERE:
				run_adhere(running_dna.get_recommended_action_location(i));
				break;
			case ActionType::INVALID:
				break;
			default:
				std::cout<<"Action type "<<static_cast<int>(running_dna.get_recommended_action(i))<<" is unknown/n";
			
		}
	}
}

bool Cell::try_to_split(){
	
	if (inside_concentrations[static_cast<int>(MoleculeType::ATP)] > 10.0){
		
		//split();
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
		case CellType::FLAGELLA:
			run_flagella_type();
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


void Cell::simulation_step(int current_cell_idx){
	//std::cout<<"start";
	if (inside_concentrations[static_cast<int>(MoleculeType::ATP)] < 4.5 || (percent_covered > 1.0 && cell_type != CellType::NONE && false)){
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
	
	Position2D dif_from_center = pos - Position2D{env->get_max_pos()/2.0, env->get_max_pos()/2.0};
	float dist = std::sqrt((dif_from_center.x*dif_from_center.x) + (dif_from_center.y*dif_from_center.y));
	
	vel.x += .0000005*dif_from_center.y/dist;
	vel.y += .0000005*-dif_from_center.x/dist;
	
	
	pos += vel;
	
	angle_rads += angle_vel;
	
	
	
	mass = 0;

	for (int i = 0; i< number_of_molecule_types; i++){
		mass += inside_concentrations[i]/4.0;
	}
	//inside_concentrations[static_cast<int>(MoleculeType::ATP)];
	
	size = sqrt(mass/M_PI) * 10;
	
	
	
	if (!start_dna.is_past_childhood()){
		start_dna.simulate_step(env->sample_pos(pos), env->sample_pos(pos + Position2D{size, size}));
		
		if (start_dna.is_past_childhood()){
			//std::cout<<"adult: "<<start_dna.get_determined_cell_type();
			cell_type = start_dna.get_determined_cell_type();
		}
	}else{
		/*if (percent_covered>.8){
			std::cout<<"die\n";
			is_dead = true;
		
			env->add_molecules_to_pos(pos, inside_concentrations);
			
			cell_manager->delete_cell(this);
					for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
				
				stuck_cells[i]->disconnect_connection();
				
			}
			return;
		}*/
	}
	
	//get touching cells in order for adhesion, doesn't need to be fast because you will really ever be touchign like 4 cells max
	std::vector<std::weak_ptr<Cell>> touching_cells = cell_manager->find_touching_cells(current_cell_idx);
	//std::cout<<"start touching cells";
	for (int i = 0; i < static_cast<int>(touching_cells.size()); i++){
		
		if (auto target_cell = touching_cells[i].lock()){
			if (!is_stuck_to_cell(target_cell.get()) && willing_for_connection(target_cell.get()) && target_cell->willing_for_connection(this)){ // if both me and the other are ready for connection, then do it

				std::shared_ptr<AdhesionConnection> new_conn(new AdhesionConnection(this, target_cell.get()));

				this->force_add_adhesion_connection(new_conn); // give ownership to the two cells
				target_cell->force_add_adhesion_connection(new_conn);

				cell_manager->add_adhesion_connection(new_conn);

			}
		}
		
	}
	
	//std::cout<<" end1\n";
	
	
	
	//cell_effiency -= abs(inside_concentrations[static_cast<int>(MoleculeType::OXYGEN)])/200000.0;
	//std::cout<<"cell_effiency: "<<cell_effiency<<"\n";
	
	if (cell_effiency < 0.0) cell_effiency = 0.0;
	
	
	run_cell_type();
	do_all_dna_actions();
	if (mass > 0){
		inside_concentrations[static_cast<int>(MoleculeType::ATP)] -= plant_cell_efficiency*mass*mass*.01;
	}
	
	percent_covered = 0.0;
	
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

bool Cell::willing_for_connection(Cell* target_cell){
	if (stuck_cells.size() >= 3 || !wants_adhere) return false;
	wants_adhere = false;
	//std::cout<<Genetics::genetic_difference(&(this->running_dna), &(this->start_dna), &(target_cell->running_dna), &(target_cell->start_dna))<<"\n";
	if (Genetics::genetic_difference(&(this->running_dna), &(this->start_dna), &(target_cell->running_dna), &(target_cell->start_dna)) > .4) return false;
	//std::cout<<Genetics::genetic_difference(&(this->running_dna), &(this->start_dna), &(target_cell->running_dna), &(target_cell->start_dna))<<" * 100 difference\n";
	
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
		case CellType::FLAGELLA:
			return true;
			break;
		
	}
	
	
	return false;
	
}

bool Cell::get_is_dead(){
	return is_dead;
}

void Cell::add_cell_overlap(float amount){
	percent_covered += std::abs(amount/(size));
}

void Cell::admin_kill_cell(){
	is_dead = true;
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
	
	
	mass = inside_concentrations[static_cast<int>(MoleculeType::ATP)];
	size = sqrt(mass/M_PI)*10;
	
	cell_manager->check_for_new_species(this);
	
	
	
}

Cell::Cell(Environment* env, CellRenderManager* cell_man, Position2D pos, MoleculeConcentrations conc, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna, int specie_parent_index) : start_dna(parent_start_dna), running_dna(parent_run_dna){
	start_dna.mutate();
	running_dna.mutate();
	
	
	
	cell_type = CellType::NONE;
	
	this->env = env;
	this->cell_manager = cell_man;
	this->pos = pos;
	this->inside_concentrations = conc;
	this->specie_parent_index = specie_parent_index;
	
	
	mass = inside_concentrations[static_cast<int>(MoleculeType::ATP)];
	size = sqrt(mass/M_PI)*10;
	
	
	
	cell_manager->check_for_new_species(this);
}


Cell::~Cell(){
	for (int i = 0; i < static_cast<int>(stuck_cells.size()); i++){
		
		stuck_cells[i]->disconnect_connection();
		
	}
}
