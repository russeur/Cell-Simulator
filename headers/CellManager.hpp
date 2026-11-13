#pragma once

#include "Cell_Render.hpp"
#include "Environment.hpp"
#include "AdhesionConnection.hpp"
#include <vector>
#include <memory>
#include <utility>
#include <iterator>

struct Specie_Node;

struct Specie_Info{
	int population;
	Specie_Node* tree_node = nullptr;
};
struct Specie_Node{
	int specie_idx;
	Specie_Node* parent;
	std::list<Specie_Node> children;
};




class CellRenderManager{
	public:
	Environment* env;
	Render_Rect_Manager<CellVert>* render_rect_man;
	Window* window;
	
	
	std::vector<std::unique_ptr<Genetics::genome>> specie_genes; // do not remove items from this list unless the manager is destoying
	std::vector<Specie_Info> all_specie_info; // in same order as specie_genes

	std::list<Specie_Node> head_specie_nodes;
	
	
	std::vector<std::shared_ptr<Cell>> cell_list; // list of (cell_ptr, list of collisions)
	std::vector<std::vector<std::weak_ptr<Cell>>> collision_list;
	
	std::vector<std::weak_ptr<AdhesionConnection>> adhesion_connections;
	
	
	
	bool is_colliding(Cell*, Cell*);
	bool fix_collision(Cell* cell_a, Cell* cell_b);
	bool fix_wall_collisions(Cell*);
	bool fix_env_wall_collisions(Cell*);
	bool fix_env_collision(Cell*, int tile_pos);
	
	public:
	
	//specie identifier stuff
	void find_init_species();
	void check_for_new_species(Cell* new_cell);
	
	
	//void add_cell(std::unique_ptr<Cell_Render>); // Cell Manager will now take care and own this pointer/cell from now on
	void add_cell(Position2D, MoleculeConcentrations);
	void add_cell(Position2D, MoleculeConcentrations, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna, int specie_parent_index);
	void delete_cell(Cell*);
	void add_adhesion_connection(std::weak_ptr<AdhesionConnection>);
	
	std::vector<std::weak_ptr<Cell>> find_touching_cells(Cell*);
	std::vector<std::weak_ptr<Cell>> find_touching_cells(int idx);
	//find light, so that eye cells can work.
	
	
	void simulation_step(GLuint m_shaderProg, GLuint BufSize, GLuint m_posBuf[2], int read_buff_idx);
	void render_step(int selected_species_id);
	
	CellRenderManager(Environment*, Render_Rect_Manager<CellVert>*, Window*);
	
};




