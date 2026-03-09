#pragma once
#include "Cell.hpp"

#include <utility>



class SpringConnection{
	Position2D next_c1_force = Position2D{0, 0};
	Position2D next_c2_force = Position2D{0, 0};
	
	float next_c1_torque = 0;
	float next_c2_torque = 0;
	
	bool is_disconnected = false;// if true, that means one of the cells connected does not want to be connected anymore, c1 and c2 are considered to be hanging now for safety
	
	public:
	
	
	
	
	double c1_angle_rads = 0;
	double c2_angle_rads = 0;
	
	Cell* c1;
	Cell* c2;
	
	
	
	
	
	Cell* get_other_cell(Cell* current_cell);
	
	Position2D get_conn_relative_location(Cell* target_cell);
	Position2D get_conn_global_location(Cell* target_cell);
	
	void apply_force_to_cell(Cell* target_cell);
	
	void disconnect_connection();
	bool get_is_disconnected();
	
	
	void simulation_step(); // should be called right before the cells use this connection every simulation step
	
	
	SpringConnection(Cell* c1, Cell* c2);
	~SpringConnection();
	
};


#include "Molecules.hpp"
class AdhesionConnection : public SpringConnection{ // used for transportation of molecules
	public:
	//void transfer_molecule(MoleculeType, float amount);
	//void transfer_molecules(MoleculeConcentrations);
	AdhesionConnection(Cell* c1, Cell* c2);
	
};