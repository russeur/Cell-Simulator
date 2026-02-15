#pragma once


#include "Environment.hpp"
#include <vector>
#include <memory>
#include <utility>


class Cell;
class Cell_Render;



class CellGroup{
	
	Position2D pos;
	float angle;
	
	Position2D vel;
	float angular_vel;
	
	float mass;
	
	float rotational_inertia;
	
	std::vector<Cell*> cells;
	
	
	
}


class CellRenderManager{
	public:
	Environment* env;
	std::vector<std::unique_ptr<Cell>> cell_list; // list of (cell_ptr, list of collisions)
	std::vector<std::vector<Cell*>> collision_list;
	
	bool is_colliding(Cell*, Cell*);
	bool fix_collision(Cell* cell_a, Cell* cell_b);
	bool fix_wall_collisions(Cell*);
	
	public:
	
	void add_cell(std::unique_ptr<Cell_Render>); // Cell Manager will now take care and own this pointer/cell from now on
	void delete_cell(Cell*);
	
	std::vector<Cell*> find_touching_cells(Cell*);
	//find light, so that eye cells can work.
	
	
	void simulation_step();
	void render_step();
	
	CellRenderManager(Environment*);
	
};




