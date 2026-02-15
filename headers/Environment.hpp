#pragma once
#include <vector>
#include <cmath>

#include "Molecules.hpp"
#include "Position2D.hpp"
#include <iostream>

class Environment{
	public:
	static constexpr int square_size = 20;
	static constexpr int world_dim = 25; //if world_dim = 3, then we will only have 9 squares and the world would be limited by 3*square_size
	
	
	std::vector<float> concentrations[number_of_molecule_types]; // size of vector will always be world_dim^2
	
	
	int get_tile_from_pos(Position2D);
	
	
	
	MoleculeConcentrations sample_pos(Position2D); //finds molecule concentrations at that location
	
	void add_molecule_to_pos(Position2D, MoleculeType, float amount); // fastest if you are only adding one molecule
	
	void add_molecules_to_pos(Position2D, MoleculeConcentrations); // will be slower than add molecule to pos if you are only adding one molecule
	
	float get_max_pos(); // max position across any dimension
	
	
	//should only be called by World
	
	void simulation_step(); // only should be called by World each simulation step
	void set_all_concentrations(MoleculeConcentrations); //sets the entire world with these molecule concentrations
	
	Environment();
	
};
