
#include "Environment.hpp"

int Environment::get_tile_from_pos(Position2D pos){
	int x_idx = std::floor(pos.x / (float)square_size);
	int y_idx = std::floor(pos.y / (float)square_size);
	
	if (x_idx < 0 || x_idx >= world_dim || y_idx < 0 || y_idx >= world_dim) return 0;
	
	return x_idx + (y_idx * world_dim);
	
}
Position2D Environment::get_pos_from_tile(int tile_pos){

	return Position2D{static_cast<float>((tile_pos%world_dim)*square_size)+static_cast<float>(square_size)/2.0, static_cast<float>((tile_pos/world_dim)*square_size)+static_cast<float>(square_size)/2.0};

}

MoleculeConcentrations Environment::sample_pos(Position2D pos){
	
	MoleculeConcentrations record;
	int tile_idx = get_tile_from_pos(pos);
	
	for (int c = 0; c < number_of_molecule_types; c++){
		record[c] = concentrations[c*world_dim*world_dim+tile_idx];
	}
	
	return record;
	
}



void Environment::add_molecule_to_pos(Position2D pos, MoleculeType m_type, float amount){
	
	concentrations[static_cast<int>(m_type)*world_dim*world_dim+get_tile_from_pos(pos)] += amount;
	
	
	
}

void Environment::add_molecules_to_pos(Position2D pos, MoleculeConcentrations amounts){
	
	int tile_idx = get_tile_from_pos(pos);
	
	for (int c = 0; c < number_of_molecule_types; c++){
		
		concentrations[c*world_dim*world_dim+tile_idx] += amounts[c];
		
	}
	
}

float Environment::get_max_pos(){
	
	return static_cast<float>(world_dim * square_size);
	
}




void Environment::simulation_step(){
	
	//TODO
	for (int c = 0; c < number_of_molecule_types; c++){
		
		float* current_conc = &concentrations[c*world_dim*world_dim];

		for (int t = 0; t < world_dim*world_dim; t++){
			
			if (c == static_cast<int>(MoleculeType::ATP)){
				current_conc[t] = 0.0;
				continue;

			}

			//bool spread_mask[4] = {0, 0, 0, 0}; // 0 for do not spread to this adj tile, 1 for do spread
			int adj_tile_idx[4] = {t+1, t-1, t+world_dim, t-world_dim};
	
			for (int adj = 0; adj < 4; adj++){
				
				if (adj_tile_idx[adj]>=0 && 
				adj_tile_idx[adj]<(world_dim*world_dim) && 
				!(t%world_dim == 0 && adj_tile_idx[adj]%world_dim == world_dim-1) && 
				!(t%world_dim == world_dim-1 && adj_tile_idx[adj]%world_dim == 0) && 
				(current_conc[adj_tile_idx[adj]] < current_conc[t])
				){
					
					
					float spread_amount = (current_conc[t] - current_conc[adj_tile_idx[adj]]) * .1;
					//std::cout<<"s: "<<spread_amount<<"\n";
					current_conc[adj_tile_idx[adj]] += spread_amount;
					current_conc[t] -= spread_amount;
					
				}
				
			}
			
			
			
			
			
			
		}
	}
	//std::cout<<"c: "<<acount<<"\n";
	
}
void Environment::set_all_concentrations(MoleculeConcentrations new_conc){
	
	for (int c = 0; c < number_of_molecule_types; c++){
		
		for (int i = 0; i< world_dim*world_dim; i++){
			concentrations[c*world_dim*world_dim+i] = new_conc[c];
		}
		
		
	}
	
}

Environment::Environment(){
	
	for (int i = 0; i < world_dim*world_dim; i++){
		concentrations[number_of_molecule_types*world_dim*world_dim + i] = 0;

	}
	
	set_all_concentrations(MoleculeConcentrations{0.0, 0.0, 0.0, 0.0, 0.0});
	
}


