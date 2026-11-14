
#pragma once
#include <array>

constexpr int number_of_molecule_types = 8;

enum class MoleculeType{
	SUGAR, //0
	ATP, //1
	GOOD_ACID, //2
 	ACID,  //3
	OXYGEN, //4
	CO2,  //5
	LIGHT_INDICATOR,
	PLANTAESE
};
constexpr const char* mol_name[number_of_molecule_types] = {"SUGAR",
					 "ATP",
	                 "GOOD_ACID",
	                 "ACID",
	                 "OXYGEN",
					 "CO2",
	                 "LIGHT_INDICATOR",
					 "PLANTEASE"   
	
};


using MoleculeConcentrations = std::array<float, number_of_molecule_types>;

enum class ConcComparisonType{
	INSIDE, 
	OUTSIDE,
	DIFF // outside-inside
};
