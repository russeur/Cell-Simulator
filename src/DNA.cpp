#include "DNA.hpp"
#include <cmath>
#include <random>
#include <iostream>
#include <cstdlib>



//
//
//
//
//
//genetic definitions
//
//

Genetics::ConcentrationTarget Genetics::get_rand_conc_target(MoleculeType mol_type){
	
	ConcentrationTarget new_target = {static_cast<ConcComparisonType>(rand()%3), mol_type, static_cast<float>(rand()%100)/25.0f};
	return new_target;
}
bool Genetics::mutate_conc_target(ConcentrationTarget* target){
	float rand_val = static_cast<float>(rand()%100) / 100;
	if (rand_val < mutate_chance){
		
		if (rand_val < mutate_chance*mutate_target_value_chance){
			target->concentration += mutate_target_value_stength * ((rand()%2) * 2 - 1);
		}else{
			target->comp_type = static_cast<ConcComparisonType>(rand()%3);
		}
		
		return true;
	}
	return false;
}
bool Genetics::is_conc_same(ConcentrationTarget t1, ConcentrationTarget t2){
	if (t1.comp_type != t2.comp_type || t1.molecule_type != t2.molecule_type || t1.concentration != t2.concentration) return false;
	
	return true;
}



//
//
//
//
//
//Starting DNA definitions
//
//


void Genetics::StartingDNA::mutate(){
	
	for (int t = 0; t < num_of_cell_types; t++){
		for (int mt = 0; mt < number_of_molecule_types; mt++){
			mutate_conc_target(&all_cell_type_targets[t].all_concentration_targets[mt]);
		}
	}
	
}

void Genetics::StartingDNA::determine_cell_type(){
	if (!is_past_childhood()){// if not past childhood, then just make it a child
		determined_cell_type = CellType::NONE;
		return;
	}	
	if (determined_cell_type != CellType::NONE) return; // if the cell already has type, then keep it that type
	
	//update type based off of tarets and dna
	
	//get averages
	MoleculeConcentrations concentration_averages[3]; // 3 for inside, outside, and diff
	for (int mt = 0; mt < number_of_molecule_types; mt++){
		concentration_averages[0][mt] = concentration_sums[0][mt] / static_cast<float>(frames_since_birth); // inside
		concentration_averages[1][mt] = concentration_sums[1][mt] / static_cast<float>(frames_since_birth); // outside
		concentration_averages[2][mt] = concentration_averages[1][mt] - concentration_averages[0][mt]; // diff
	}
	
	
	float cell_sum_squared_dif[num_of_cell_types];
	for (int t = 0; t < num_of_cell_types; t++){
		cell_sum_squared_dif[t] = 0;
	}
	
	for (int t = 0; t < num_of_cell_types; t++){
		for (int mt = 0; mt < number_of_molecule_types; mt++){
			float dif = all_cell_type_targets[t].all_concentration_targets[mt].concentration - concentration_averages[static_cast<int>(all_cell_type_targets[t].all_concentration_targets[mt].comp_type)][mt];
			cell_sum_squared_dif[t] += dif*dif;
		}
	}
	
	int closest_celltype = 0; // will need to be increased by one because num_of_cell_types doesn't include NONE
	float closest_dist = std::sqrt(cell_sum_squared_dif[0]);
	//std::cout<< "dist: "<<closest_dist<<"\n";
	
	
	for (int t = 1; t < num_of_cell_types; t++){
		float dist = std::sqrt(cell_sum_squared_dif[t]);
		//std::cout<< "dist: "<<dist<<"\n";
		if (dist < closest_dist){
			closest_celltype = t;
			closest_dist = dist;
			
		}
		
	}
	
	determined_cell_type = static_cast<CellType>(closest_celltype+1); // increased by one because num_of_cell_types doesn't include NONE
	
}

void Genetics::StartingDNA::reset_life(){
	
	concentration_sums[0] = MoleculeConcentrations{0,0,0,0,0};
	concentration_sums[1] = MoleculeConcentrations{0,0,0,0,0};
	
	frames_since_birth = 0;
	determined_cell_type = CellType::NONE;
	
}

bool Genetics::StartingDNA::is_past_childhood(){
	
	return frames_since_birth > childhood_duration;
	
}

void Genetics::StartingDNA::simulate_step(MoleculeConcentrations inside_conc, MoleculeConcentrations outside_conc){
	if (is_past_childhood()) return;
	
	
	frames_since_birth++;
	for (int i = 0; i < number_of_molecule_types; i++){
		concentration_sums[0][i] += inside_conc[i];
		concentration_sums[1][i] += outside_conc[i];
	}
	
	if (is_past_childhood()){
		determine_cell_type();
	}
	
}

CellType Genetics::StartingDNA::get_determined_cell_type(){
	return determined_cell_type;
}

const Genetics::StartingDNA::CellTypeTargets* Genetics::StartingDNA::get_targets()  const{
	
	return &all_cell_type_targets[0];
	
}

Genetics::StartingDNA::StartingDNA(const StartingDNA& parent_dna){
	
	
	for (int t = 0; t < num_of_cell_types; t++){
		all_cell_type_targets[t].cell_type = parent_dna.get_targets()[t].cell_type;	
		for (int mt = 0; mt < number_of_molecule_types; mt++){
			
			all_cell_type_targets[t].all_concentration_targets[mt] = parent_dna.get_targets()[t].all_concentration_targets[mt];
			
		}
	}
	
	concentration_sums[0] = MoleculeConcentrations{0,0,0,0,0};
	concentration_sums[1] = MoleculeConcentrations{0,0,0,0,0};
	
}
Genetics::StartingDNA::StartingDNA(bool random_dna){
	
	for (int t = 0; t < num_of_cell_types; t++){
		all_cell_type_targets[t].cell_type = static_cast<CellType>(t+1); // +1 because num_of_cell_types doesn't include NONE
		
		for (int mt = 0; mt < number_of_molecule_types; mt++){
			
			all_cell_type_targets[t].all_concentration_targets[mt] = get_rand_conc_target(static_cast<MoleculeType>(mt));
			
		}
		
	}
	
	concentration_sums[0] = MoleculeConcentrations{0,0,0,0,0};
	concentration_sums[1] = MoleculeConcentrations{0,0,0,0,0};
	
}






//
//
//
//
//
//
//
//
//
//
//
//
//
//
//Running DNA definitions
//
//

Genetics::RunningDNA::ActionTypeTarget Genetics::RunningDNA::get_rand_action_target(){
	ActionTypeTarget new_target = 	{
									get_rand_conc_target(static_cast<MoleculeType>(rand()%number_of_molecule_types)), 
									static_cast<float>(rand()%100)/25.0f, 
									static_cast<ActionType>(rand()%num_of_actions),
									static_cast<ActionLocation>(rand()%num_of_action_locations)
									};
	return new_target;
}
bool Genetics::RunningDNA::mutate_action_target(ActionTypeTarget* target){
	
	const float mutate_chance = .05;
	
	const float mutate_conc_molecule = .25;
	const float mutate_conc_range = .25;
	const float mutate_conc_range_strength = .1;
	const float mutate_action_type = .25;
	const float mutate_action_location = .25;
	
	
	mutate_conc_target(&target->concentration_target);
	
	float rand_val = static_cast<float>(rand()%100) / 100;
	if (rand_val < mutate_chance){
		float prob_offset = 0;
		if (rand_val < (prob_offset+=mutate_chance*mutate_conc_molecule)){ // change molecule type 
			target->concentration_target.molecule_type = static_cast<MoleculeType>(rand()%number_of_molecule_types);
		}
		else if (rand_val < (prob_offset+=mutate_chance*mutate_conc_range)){ // change conc range
			target->target_range += mutate_conc_range_strength * ((rand()%2) * 2 - 1);
		}
		else if (rand_val < (prob_offset+=mutate_chance*mutate_action_type)){ // change action type
			target->action_type = static_cast<ActionType>(rand()%num_of_actions);
		}
		else{ //change action location
			target->action_location = static_cast<ActionLocation>(rand()%num_of_action_locations);
		}
	}
	
	return false;
}


void Genetics::RunningDNA::mutate(){
	
	for (int target_i = 0; target_i < static_cast<int>(all_action_targets.size()); target_i++){
		
		mutate_action_target(&all_action_targets[target_i]);
		
	}
	
	const float mutate_add_action_chance = .05;
	if (static_cast<float>(rand()%100)/100 < mutate_add_action_chance){
		
		if (static_cast<float>(rand()%100)/100.0 < .5) all_action_targets.push_back(get_rand_action_target());
		else if (static_cast<int>(all_action_targets.size()) != 0) all_action_targets.erase(all_action_targets.begin()+(rand()%static_cast<int>(all_action_targets.size())));
	}
}

void Genetics::RunningDNA::simulate_step(MoleculeConcentrations inside_conc, MoleculeConcentrations outside_conc){
	
	MoleculeConcentrations diff_conc;
	for (int m = 0; m < number_of_molecule_types; m++){
		diff_conc[m] = outside_conc[m] - inside_conc[m];
	}
	
	current_recommended_actions.clear();
	
	for (int target_i = 0; target_i < static_cast<int>(all_action_targets.size()); target_i++){
		
		//find the value in the real world so that we can compare to the target value range
		float actual_value = get_action_target_actual(target_i, diff_conc, inside_conc, outside_conc);
		
		if (actual_value < all_action_targets[target_i].concentration_target.concentration + all_action_targets[target_i].target_range &&
			actual_value > all_action_targets[target_i].concentration_target.concentration - all_action_targets[target_i].target_range){ // check both the lower and upper bounds of taking this action
			current_recommended_actions.push_back(target_i);
		}
		
	}
	
}

float Genetics::RunningDNA::get_action_target_actual(int action_idx, MoleculeConcentrations diff_conc, MoleculeConcentrations inside_conc, MoleculeConcentrations outside_conc){
	
	
	if (action_idx >= all_action_targets.size()) return 0;
	//find the value in the real world so that we can compare to the target value range
	float actual_value;
	switch (all_action_targets[action_idx].concentration_target.comp_type){ // get value based on what type of comparison type it is (e.i. value from inside, outside or difference)
		case ConcComparisonType::INSIDE:
			actual_value = inside_conc[static_cast<int>(all_action_targets[action_idx].concentration_target.molecule_type)];
			break;
		case ConcComparisonType::OUTSIDE:
			actual_value = outside_conc[static_cast<int>(all_action_targets[action_idx].concentration_target.molecule_type)];
			break;
		case ConcComparisonType::DIFF:
			actual_value = diff_conc[static_cast<int>(all_action_targets[action_idx].concentration_target.molecule_type)];
			break;
		default:
			
			std::cout<<"Action comparison type is not either INSIDE, OUTSIDE, or DIFF; it is "<<static_cast<int>(all_action_targets[action_idx].concentration_target.comp_type)<<"\n";
			break;
	}
	return actual_value;


}

ActionType Genetics::RunningDNA::get_recommended_action(int action_index){
	if (action_index >= current_recommended_actions.size() || current_recommended_actions[action_index] >= all_action_targets.size()) return ActionType::INVALID;
	return all_action_targets[current_recommended_actions[action_index]].action_type;
}
ActionLocation Genetics::RunningDNA::get_recommended_action_location(int action_index){
	if (action_index >= current_recommended_actions.size() || current_recommended_actions[action_index] >= all_action_targets.size()) return ActionLocation::INSIDE;
	return all_action_targets[current_recommended_actions[action_index]].action_location;
}
int Genetics::RunningDNA::get_number_of_recommended_actions(){
	return static_cast<int>(current_recommended_actions.size());
}

const Genetics::RunningDNA::ActionTypeTarget* Genetics::RunningDNA::get_targets() const{
	
	return all_action_targets.data(); 
}
int Genetics::RunningDNA::get_number_of_targets() const{
	return static_cast<int>(all_action_targets.size());
}

Genetics::RunningDNA::RunningDNA(const RunningDNA& parent_dna){
	
	all_action_targets.resize(parent_dna.get_number_of_targets());
	
	for (int gene_i = 0; gene_i < static_cast<int>(all_action_targets.size()); gene_i++){
		
		all_action_targets[gene_i] = parent_dna.get_targets()[gene_i];
		
	}
	
}

Genetics::RunningDNA::RunningDNA(){
	
	all_action_targets.resize(4);
	
	ActionTypeTarget new_target = 	{
									ConcentrationTarget{ConcComparisonType::INSIDE, MoleculeType::ATP, 10.0}, 
									8.0, 
									ActionType::SPLIT,
									ActionLocation::INSIDE
									};
	
	all_action_targets[0] = new_target;

	ActionTypeTarget new_target2 = 	{
									ConcentrationTarget{ConcComparisonType::INSIDE, MoleculeType::SUGAR, 8.0}, 
									2.0, 
									ActionType::SUGAR_TO_ATP,
									ActionLocation::INSIDE
									};
	
	
	all_action_targets[1] = new_target2;

	ActionTypeTarget new_target3 = 	{
									ConcentrationTarget{ConcComparisonType::INSIDE, MoleculeType::SUGAR, 6.0}, 
									2.0, 
									ActionType::OUTCO2_TO_CO2,
									ActionLocation::INSIDE
									};
	
	
	all_action_targets[2] = new_target3;

	
	
	
}

Genetics::RunningDNA::RunningDNA(bool random_dna){
	
	int number_of_targets = static_cast<int>(rand()%5);
	
	all_action_targets.resize(number_of_targets);
	
	
	for (int gene_i = 0; gene_i < number_of_targets; gene_i++){
		
		all_action_targets[gene_i] = get_rand_action_target();
		
	}
	
	
}


float Genetics::genetic_difference(RunningDNA* running1, StartingDNA* start1, RunningDNA* running2, StartingDNA* start2){
	const RunningDNA::ActionTypeTarget* running_dna_values1 = running1->get_targets();
	int running_dna_size1 = running1->get_number_of_targets();
	const RunningDNA::ActionTypeTarget* running_dna_values2 = running2->get_targets();
	int running_dna_size2 = running2->get_number_of_targets();
	const StartingDNA::CellTypeTargets* start_dna_values1 = start1->get_targets();
	const StartingDNA::CellTypeTargets* start_dna_values2 = start2->get_targets();
	
	
	float running_diff = 0;
	for (int i = 0; i < std::min(running_dna_size1, running_dna_size2); i++){
		running_diff += (Genetics::is_conc_same(running_dna_values1->concentration_target, running_dna_values2->concentration_target)) ? 0 : .25;
		running_diff += std::abs(running_dna_values1->target_range - running_dna_values2->target_range) / (4.0f * 4.0f);
		running_diff += (running_dna_values1->action_type == running_dna_values2->action_type) ? 0 : .25;
		running_diff += (running_dna_values1->action_location == running_dna_values2->action_location) ? 0 : .25;
		running_dna_values1++;running_dna_values2++;
	}
	
	running_diff += std::abs(running_dna_size1 - running_dna_size2);
	if (running_diff != 0) running_diff/=std::max(running_dna_size1, running_dna_size2);
	
	
	
	float starting_diff = 0;
	for (int i = 0; i < num_of_cell_types; i++){
		starting_diff += (start_dna_values1->cell_type == start_dna_values2->cell_type) ? 0 : 1.0;
		for (int j = 0; j < number_of_molecule_types; j++){
			starting_diff += (Genetics::is_conc_same(start_dna_values1->all_concentration_targets[j], start_dna_values2->all_concentration_targets[j])) ? 0 : 1.0/static_cast<float>(number_of_molecule_types);
		}
		start_dna_values1++;start_dna_values2++;
	}
	starting_diff /= num_of_cell_types;
	
	
	
	return running_diff*.7 + starting_diff*.7;
}


float Genetics::get_genetic_length(RunningDNA* running, StartingDNA* start){
	int sum = running->get_number_of_targets()*3 + num_of_cell_types*number_of_molecule_types/3;
	return static_cast<float>(sum);
	
	
}


