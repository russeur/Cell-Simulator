#pragma once

#include "Cell_Render.hpp"
#include "Environment.hpp"
#include "AdhesionConnection.hpp"
#include <vector>
#include <memory>
#include <utility>








class CellRenderManager{
	public:
	Environment* env;
	Render_Rect_Manager<CellVert>* render_rect_man;
	Window* window;
	
	
	std::vector<std::unique_ptr<Cell>> cell_list; // list of (cell_ptr, list of collisions)
	std::vector<std::vector<Cell*>> collision_list;
	
	std::vector<std::weak_ptr<AdhesionConnection>> adhesion_connections;
	
	
	
	bool is_colliding(Cell*, Cell*);
	bool fix_collision(Cell* cell_a, Cell* cell_b);
	bool fix_wall_collisions(Cell*);
	
	public:
	
	//void add_cell(std::unique_ptr<Cell_Render>); // Cell Manager will now take care and own this pointer/cell from now on
	void add_cell(Position2D, MoleculeConcentrations);
	void add_cell(Position2D, MoleculeConcentrations, const Genetics::StartingDNA& parent_dna);
	void delete_cell(Cell*);
	void add_adhesion_connection(std::weak_ptr<AdhesionConnection>);
	
	std::vector<Cell*> find_touching_cells(Cell*);
	//find light, so that eye cells can work.
	
	
	void simulation_step();
	void render_step();
	
	CellRenderManager(Environment*, Render_Rect_Manager<CellVert>*, Window*);
	
};




