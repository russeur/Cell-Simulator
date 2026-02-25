
#pragma once

#include "Molecules.hpp"
#include "Environment.hpp"

#include <memory>
#include <cmath>


class CellRenderManager;
class AdhesionConnection;


enum CellType{
	NONE,
	EATER,
	PLANT
	
};









class Cell{
	
	void run_plant_type();
	void split();
	
	virtual void notify_add_connection() = 0;
	virtual void notify_remove_connection(int i) = 0;
	
	
	protected:
	
	bool try_to_split();
	void run_cell_type();
	
	
	
	
	public:
	
	Environment* env;
	CellRenderManager* cell_manager;
	CellType cell_type = CellType::NONE; // chosen from starting_genes
	float size = 0; // based solely on food inside of cell
	float mass = 0; // mass and volume in this case are equivelent
	
	Position2D pos = Position2D{0, 0}; // based on center of cell/circle
	double angle_rads = 0; // in radians
	
	Position2D vel = Position2D{0.0, 0.0};
	double angle_vel = 0; // in rans
	
	
	
	
	
	
	
	
	
	std::vector<std::shared_ptr<AdhesionConnection>> stuck_cells; // list of all cells stuck to this one
	
	
	MoleculeConcentrations inside_concentrations;
	
	//TODO: starting and running genes
	
	void add_force(Position2D force);
	void add_torque(float torque);
	
	void simulation_step();
	bool is_stuck_to_cell(Cell*);
	void force_add_adhesion_connection(std::shared_ptr<AdhesionConnection>);
	
	bool willing_for_connection(); // true if it can connect, false if not
	
	Cell(Environment*, CellRenderManager*);
	Cell(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations);
	
	
	
	
	
	
	
};



