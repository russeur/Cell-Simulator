
#pragma once
#include <array>

constexpr int number_of_molecule_types = 5;

enum MoleculeType{
	FOOD,
	GOOD_ACID,
	ACID,
	OXYGEN,
	LIGHT_INDICATOR
	
	
};


using MoleculeConcentrations = std::array<float, number_of_molecule_types>;


