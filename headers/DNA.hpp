#pragma once  

#include "Molecules.hpp"


constexpr int num_of_cell_types = 2; // excludes NONE cell
enum CellType{
	NONE,
	EATER,
	PLANT
	
};



namespace Genetics{
	constexpr float mutate_chance = .2; // 0.0-1.0, chance per molecule target
	constexpr float mutate_target_value_chance = .8; // 0.0-1.0, chance for a quantitiy target to be changed if mutating. 1-mutate_target_value_chance = mutate_comparison_type_chance
	constexpr float mutate_target_value_stength = .1; // the amount a quantity target will change if mutated
	struct ConcentrationTarget{
		ConcComparisonType comp_type;
		MoleculeType molecule_type;
		float concentration;
		
	};
	ConcentrationTarget get_rand_conc_target(MoleculeType);
	void mutate_conc_target(ConcentrationTarget*);
	
	
	constexpr int childhood_duration = 180; //number of frames it takes for a cell from birth to get a celltype
	
	
	
	
	class StartingDNA{
		
		struct CellTypeTargets{
			
			ConcentrationTarget all_concentration_targets[number_of_molecule_types];
			CellType cell_type;
			
		};
		//
		CellTypeTargets all_cell_type_targets[num_of_cell_types];
		
		
		//sums are used to find averages since birth to adulthood
		MoleculeConcentrations concentration_sums[2];//2 because there is inside and outside
		
		int frames_since_birth = 0;
		
		CellType determined_cell_type = CellType::NONE;
		void determine_cell_type(); // determine it based on targets
		
		public:
		
		void mutate();
		
		void reset_life(); // used for after splitting
		
		bool is_past_childhood();
		
		void simulate_step(MoleculeConcentrations inside_conc, MoleculeConcentrations outside_conc); //no need to call this function if it is pastchildhood
		
		CellType get_determined_cell_type(); //  returns NONE if not past childhood, returns the cell type the dna wants otherwise
		
		const CellTypeTargets* get_targets() const;
		
		StartingDNA(const StartingDNA&); // copy the dna into this one
		StartingDNA(bool random_dna); // true will give you random dna, false will give you all 0s
		
		
	};
};