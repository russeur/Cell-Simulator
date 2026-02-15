
#pragma once

#include "Molecules.hpp"
#include "Environment.hpp"
#include "CellManager.hpp"
#include <cmath>

enum CellType{
	EATER
	
};
class Cell;

struct AdhesionConnection{
	Cell* cell;
	double host_angle_rads = 2;
	double client_angle_rads = 2;
	
};

class CellRenderManager;




class Cell{
	
	public:
	
	Environment* env;
	CellRenderManager* cell_manager;
	CellType cell_type; // chosen from starting_genes
	float size; // based solely on food inside of cell
	Position2D pos; // based on center of cell/circle
	Position2D vel = {0.0, 0.0}; 
	

	
	double angle_rads = 0;
	double angle_velocity = 0;
	
	std::vector<AdhesionConnection> stuck_cells; // list of all cells stuck to this one
	
	
	MoleculeConcentrations inside_concentrations;
	
	//TODO: starting and running genes
	
	
	void simulation_step();
	bool is_stuck_to_cell(Cell*);
	
	Cell(Environment*, CellRenderManager*);
	Cell(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations);
	
	
	
	
	
	
	
};



