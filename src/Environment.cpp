
#include "Environment.hpp"

int Environment::get_tile_from_pos(Position2D pos){
	int x_idx = std::floor(pos.x / (float)square_size);
	int y_idx = std::floor(pos.y / (float)square_size);
	
	
	return x_idx + (y_idx * world_dim);
	
}

MoleculeConcentrations Environment::sample_pos(Position2D pos){
	
	MoleculeConcentrations record;
	int tile_idx = get_tile_from_pos(pos);
	
	for (int c = 0; c < number_of_molecule_types; c++){
		record[c] = concentrations[c][tile_idx];
	}
	
	return record;
	
}



void Environment::add_molecule_to_pos(Position2D pos, MoleculeType m_type, float amount){
	
	concentrations[static_cast<int>(m_type)][get_tile_from_pos(pos)] += amount;
	
	
	
}

void Environment::add_molecules_to_pos(Position2D pos, MoleculeConcentrations amounts){
	
	int tile_idx = get_tile_from_pos(pos);
	
	for (int c = 0; c < number_of_molecule_types; c++){
		
		concentrations[c][tile_idx] += amounts[c];
		
	}
	
}

float Environment::get_max_pos(){
	
	return static_cast<float>(world_dim * square_size);
	
}




void Environment::simulation_step(){
	float acount = 0;
	//TODO
	for (int c = 0; c < number_of_molecule_types; c++){
		
		for (int t = 0; t < world_dim*world_dim; t++){
			if (c==0){
				acount+= concentrations[c][t];
			}
			int spread_count = 0;
			bool spread_mask[4] = {0, 0, 0, 0}; // 0 for do not spread to this adj tile, 1 for do spread
			int adj_tile_idx[4] = {t+1, t-1, t+world_dim, t-world_dim};
	
			for (int adj = 0; adj < 4; adj++){
				
				if (adj_tile_idx[adj]>=0 && 
				adj_tile_idx[adj]<(world_dim*world_dim) && 
				!(t%world_dim == 0 && adj_tile_idx[adj]%world_dim == world_dim-1) && 
				!(t%world_dim == world_dim-1 && adj_tile_idx[adj]%world_dim == 0) && 
				(concentrations[c][adj_tile_idx[adj]] < concentrations[c][t])
				){
					
					spread_count++;
					spread_mask[adj] = 1;
					
				}
				
			}
			
			for (int adj = 0; adj < 4; adj++){
				
				if (spread_mask[adj] != 1) continue;
				
				float spread_amount = (concentrations[c][t] - concentrations[c][adj_tile_idx[adj]]) * .01;
				//std::cout<<"s: "<<spread_amount<<"\n";
				concentrations[c][adj_tile_idx[adj]] += spread_amount;
				concentrations[c][t] -= spread_amount;
				//std::cout<<"a: "<<adj<<"\n";
				
			}
			
			
			
		}
	}
	//std::cout<<"c: "<<acount<<"\n";
	
}
void Environment::set_all_concentrations(MoleculeConcentrations new_conc){
	
	for (int c = 0; c < number_of_molecule_types; c++){
		
		for (int i = 0; i< static_cast<int>(concentrations[c].size()); i++){
			concentrations[c][i] = new_conc[c];
		}
		
		
	}
	
}

Environment::Environment(){
	
	for (int i = 0; i < number_of_molecule_types; i++){
		
		concentrations[i].resize(world_dim*world_dim);
		
	}
	
	set_all_concentrations(MoleculeConcentrations{0.0, 0.0, 0.0, 0.0, 0.0});
	
}


