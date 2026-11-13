#pragma once  
#include <vector>
#include "Molecules.hpp"


constexpr int num_of_cell_types = 3; // excludes NONE cell
enum class CellType{
	NONE,
	EATER,
	PLANT,
	FLAGELLA
	
};

constexpr int num_of_actions = 11;
enum class ActionType{
	OXY_TO_ACID, //convert oxygen to acid
	SPLIT, // convert food to oxygen
	FOOD_TO_FOOD,
	OXY_TO_OXY,
	OUTOXY_TO_OXY,
	CO2_TO_CO2,
	OUTCO2_TO_CO2,
	ATP_TO_FLAGELLA,
	MAKE_PLANTAESE,
	SUGAR_TO_ATP,
	RUN_ADHERE,

	

	INVALID
};
constexpr char* action_name[num_of_actions] = {
	"OXY_TO_ACID",
	"SPLIT",
	"FOOD_TO_FOOD",
	"OXY_TO_OXY",
	"OUTOXY_TO_OXY",
	"CO2_TO_CO2",
	"OUTCO2_TO_CO2",
	"ATP_TO_FLAGELLA",
	"MAKE_PLANTAESE",
	"SUGAR_TO_ATP",
	"RUN_ADHERE"
	
};

constexpr int num_of_action_locations = 3; // only ever 3 valid locations, NONE does not count
enum class ActionLocation{
	INSIDE,
	OUTSIDE,
	ATTACHED, // outputs action inside of other cells
	NONE // the action does not involve doing it at a location
};



namespace Genetics{
	constexpr float mutate_chance = .05; // 0.0-1.0, chance per molecule target
	constexpr float mutate_target_value_chance = .8; // 0.0-1.0, chance for a quantitiy target to be changed if mutating. 1-mutate_target_value_chance = mutate_comparison_type_chance
	constexpr float mutate_target_value_stength = .1; // the amount a quantity target will change if mutated
	struct ConcentrationTarget{
		ConcComparisonType comp_type;
		MoleculeType molecule_type;
		float concentration;
		
	};
	ConcentrationTarget get_rand_conc_target(MoleculeType);
	bool mutate_conc_target(ConcentrationTarget*);
	bool is_conc_same(ConcentrationTarget, ConcentrationTarget);
	
	
	constexpr int childhood_duration = 180; //number of frames it takes for a cell from birth to get a celltype
	
	
	
	
	class StartingDNA{
		public:
		struct CellTypeTargets{
			
			ConcentrationTarget all_concentration_targets[number_of_molecule_types];
			CellType cell_type;
			
		};
		private:
		//
		CellTypeTargets all_cell_type_targets[num_of_cell_types]; //the actual dna that is passed down from parents and to children
		
		
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

	class RunningDNA{
		
		
		//static constexpr 
		public:
		struct ActionTypeTarget{
			
			ConcentrationTarget concentration_target; // middle of area that activate the action
			float target_range; // from concerntration target-range to concerntration target+range, the action will happen
			ActionType action_type;
			ActionLocation action_location; // output location of the action, the input is always from the inside
		};
		private:
		
		static ActionTypeTarget get_rand_action_target();
		static bool mutate_action_target(ActionTypeTarget*);
		
		std::vector<ActionTypeTarget> all_action_targets;  //the actual dna that is passed down from parents and to children
		
		std::vector<int> current_recommended_actions; //contains the index of the running_dna/all_action_targets that the cell should run
		
		public:
		
		void mutate();
		
		void simulate_step(MoleculeConcentrations inside_conc, MoleculeConcentrations outside_conc); // this is all that is required every frame to call, all other funtions get values
		
		float get_action_target_actual(int action_idx, MoleculeConcentrations diff_conc, MoleculeConcentrations inside_conc, MoleculeConcentrations outside_conc);

		ActionType get_recommended_action(int action_index); 
		ActionLocation get_recommended_action_location(int action_index);
		int get_number_of_recommended_actions();
		
		const ActionTypeTarget* get_targets() const;
		int get_number_of_targets() const;
		
		RunningDNA(const RunningDNA&); // copy the dna into this one
		RunningDNA();//give safe dna
		RunningDNA(bool random_dna); // true will give you random dna, false will give you all 0s
		
	};


	float genetic_difference(RunningDNA* running1, StartingDNA* start1, RunningDNA* running2, StartingDNA* start2);
	float get_genetic_length(RunningDNA* running, StartingDNA* start);
	
	struct genome{
		StartingDNA starting_dna;
		RunningDNA running_dna;
	};
};



