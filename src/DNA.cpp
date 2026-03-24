#include "DNA.hpp"
#include <cmath>
#include <random>
#include <iostream>
#include <cstdlib>


Genetics::ConcentrationTarget Genetics::get_rand_conc_target(MoleculeType mol_type){
	
	ConcentrationTarget new_target = {static_cast<ConcComparisonType>(rand()%3), mol_type, static_cast<float>(rand()%100)/25.0};
	return new_target;
}
void Genetics::mutate_conc_target(ConcentrationTarget* target){
	float rand_val = static_cast<float>(rand()%100) / 100;
	if (rand_val < mutate_chance){
		
		if (rand_val < mutate_chance*mutate_target_value_chance){
			target->concentration += mutate_target_value_stength * ((rand()%2) * 2 - 1);
		}else{
			target->comp_type = static_cast<ConcComparisonType>(rand()%3);
		}
		
	}
	
}

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
