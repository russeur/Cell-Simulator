
#pragma once
#include <array>

constexpr int number_of_molecule_types = 5;

enum class MoleculeType{
	FOOD,
	GOOD_ACID,
	ACID,
	OXYGEN,
	LIGHT_INDICATOR
	
	
};


using MoleculeConcentrations = std::array<float, number_of_molecule_types>;

enum class ConcComparisonType{
	INSIDE, 
	OUTSIDE,
	DIFF // outside-inside
};
