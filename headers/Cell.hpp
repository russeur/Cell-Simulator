
#pragma once

#include "Molecules.hpp"
#include "Environment.hpp"
#include "DNA.hpp"

#include <memory>
#include <cmath>


class CellRenderManager;
class AdhesionConnection;










constexpr float plant_cell_efficiency = .0014;

class Cell{
	
	void run_plant_type();
	void run_eater_type();
	void split();
	
	
	void run_oxy_to_acid(ActionLocation destination);
	void run_food_to_oxy(ActionLocation destination);
	void do_all_dna_actions();
	
	virtual void notify_add_connection() = 0;
	virtual void notify_remove_connection(int i) = 0;
	
	bool is_dead = false; //if true then this cell shouldn't be able to do anything and is really just waiting to be safely freed by manager
	
	
	protected:
	
	float cell_effiency = 1.0; // very general metric that will mostly only be lowered by bad things, like overexposure to OXYGEN
	
	bool try_to_split(); 
	void run_cell_type();
	
	
	Genetics::StartingDNA start_dna;
	Genetics::RunningDNA running_dna;
	
	public:
	
	
	
	Environment* env;
	CellRenderManager* cell_manager;
	CellType cell_type = CellType::NONE; // chosen from starting_genes
	float size = 0; // based solely on food inside of cell
	float mass = 0; // mass and volume in this case are equivelent
	
	Position2D pos = Position2D{0, 0}; // based on center of cell/circle
	float angle_rads = 0; // in radians
	
	Position2D vel = Position2D{0.0, 0.0};
	float angle_vel = 0; // in rans
	
	
	
	
	
	
	
	
	
	std::vector<std::shared_ptr<AdhesionConnection>> stuck_cells; // list of all cells stuck to this one
	
	
	MoleculeConcentrations inside_concentrations;
	
	//TODO: starting and running genes
	
	void add_force(Position2D force);
	void add_torque(float torque);
	
	void simulation_step();
	bool is_stuck_to_cell(Cell*);
	void force_add_adhesion_connection(std::shared_ptr<AdhesionConnection>);
	
	
	bool willing_for_connection(); // true if it can connect, false if not
	bool get_is_dead();
	
	Cell(Environment*, CellRenderManager*);
	Cell(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations);
	Cell(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna);
	virtual ~Cell();
	
	
	
	
	
};



