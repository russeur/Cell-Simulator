

#include <windows.h>
#include "Window.hpp"
#include <ctime>
#include <fstream>
#include <string>
#include <unordered_map>
#include <memory>

#include "Cell_Render.hpp"


struct heat_map_vert{
	
	GLfloat x, y;
	constexpr static GLfloat z = 0.0f;
	
	float heat;
	
};

RectangleID heat_ids[Environment::world_dim*Environment::world_dim];

void run_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager);

void render_simulation();


int main() {
	
	
	
	std::unordered_map<int*, int> map;
	
	std::unique_ptr<int> my_int = std::make_unique<int>(0);
	std::cout<<"set value of unique_ptr to: "<<*my_int<<"\n";
	std::cout<<"allocated memory\n";
	
	*my_int = 5;
	std::cout<<"set value of unique_ptr to: "<<*my_int<<"\n";
	
	
	
	
	
	
	map.emplace(my_int.get(), *my_int);
	
	
	int* func_arg = my_int.get();
	
	std::cout<<"my map output: "<<map[func_arg]<<"\n";
	
	
	
	
	
	
	
	
	
	
	stbi_set_flip_vertically_on_load(true);

	
	Window* window = Window::init_window(500, 500, "Cells", "UNSIZABLE");
	window->set_clear_color(Color{0.0f, 0.0f, 0.0f, 1.0f}); // set background color
	
	{
	//make envirnoment/simulation
	
	Environment env;
	
	env.set_all_concentrations(MoleculeConcentrations{.5, 0.0, .1, 0.0});
	
	Render_Rect_Manager<heat_map_vert> rect_manager("../src/shaders/heatmap_vert.txt", "../src/shaders/heatmap_frag.txt");
	
	rect_manager.start_init();
	rect_manager.link_attribute(1, GL_INT, 1, offsetof(heat_map_vert, heat));
	rect_manager.finish_init();
	
	
	for (int t = 0; t < env.world_dim*env.world_dim; t++){
		
		int x_idx = t%env.world_dim;
		int y_idx = (t-x_idx) / env.world_dim;
		
		heat_ids[t] = rect_manager.new_rect(window->screen_to_UV_pos(Position2D{x_idx * env.square_size, y_idx * env.square_size}), window->screen_to_UV_vector(Position2D{env.square_size, env.square_size}), 0.0f);
		
	}
	
	
	Render_Rect_Manager<CellVert> cell_rect_manager("../src/shaders/cell_vert.txt", "../src/shaders/cell_frag.txt");
	
	CellRenderManager cell_manager(&env);
	
	//initial cells
	cell_manager.add_cell(std::unique_ptr<Cell_Render>(new Cell_Render(&env, &cell_manager, Position2D{100.0f, 100.0f}, MoleculeConcentrations{50.0, 0.0, 0.0, 0.0}, &cell_rect_manager, window)));
	cell_manager.add_cell(std::unique_ptr<Cell_Render>(new Cell_Render(&env, &cell_manager, Position2D{270.0f, 300.0f}, MoleculeConcentrations{30.0, 0.0, 0.0, 0.0}, &cell_rect_manager, window)));
	cell_manager.add_cell(std::unique_ptr<Cell_Render>(new Cell_Render(&env, &cell_manager, Position2D{100.0f, 350.0f}, MoleculeConcentrations{20.0, 0.0, 0.0, 0.0}, &cell_rect_manager, window)));
	cell_manager.add_cell(std::unique_ptr<Cell_Render>(new Cell_Render(&env, &cell_manager, Position2D{450.0f, 350.0f}, MoleculeConcentrations{10.0, 0.0, 0.0, 0.0}, &cell_rect_manager, window)));
	
	
	
	
	cell_manager.cell_list[0]->vel = Position2D{1.0, 1};
	cell_manager.cell_list[1]->vel = Position2D{-0.0, -1};
	cell_manager.cell_list[2]->vel = Position2D{1.0, -1};
	cell_manager.cell_list[3]->vel = Position2D{1.0, 1};
	
	
	
	
	
	
	
	
	
	cell_rect_manager.start_init();
	cell_rect_manager.link_attribute(1, GL_FLOAT, 2, offsetof(CellVert, tx));
	cell_rect_manager.link_attribute(2, GL_FLOAT, 3, offsetof(CellVert, r));
	cell_rect_manager.finish_init();
	
	
	while(!window->window_should_close()){
		
		
		
		
		
		run_simulation(window, &env, &rect_manager, &cell_rect_manager, &cell_manager);
		
		render_simulation();
		
		
		
	}
	}
	
	
	window->terminate();
	
	return 0;
}

int mtype = 0;
bool l_pressed = false;
bool r_pressed = false;
bool left_pressed = false;
void run_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager){
	
	
	
	
	
	
	if (!l_pressed && (window->get_key_button_state(GLFW_KEY_LEFT) == GLFW_PRESS)){
		mtype--;
		l_pressed = true;
	}
	else if (window->get_key_button_state(GLFW_KEY_LEFT) == GLFW_RELEASE) l_pressed = false;
	
	if (!r_pressed && (window->get_key_button_state(GLFW_KEY_RIGHT) == GLFW_PRESS)){
		mtype++;
		r_pressed = true;
	}
	else if (window->get_key_button_state(GLFW_KEY_RIGHT) == GLFW_RELEASE) r_pressed = false;
	
	
	mtype = std::max(mtype, 0);
	mtype = std::min(mtype, number_of_molecule_types-1);
	
	
	if (window->get_mouse_button_state(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		if (!left_pressed){
			cell_manager->add_cell(std::unique_ptr<Cell_Render>(new Cell_Render(env, cell_manager, Position2D{window->cursor_x, window->cursor_y}, MoleculeConcentrations{10.0, 0.0, 0.0, 0.0}, cell_rect_manager, window)));
		}
		//env->add_molecule_to_pos(Position2D{window->cursor_x, window->cursor_y}, static_cast<MoleculeType>(mtype), .1);
		env->add_molecules_to_pos(Position2D{window->cursor_x, window->cursor_y}, MoleculeConcentrations{-.2, .2, .2, 1.0});
		left_pressed = true;
	}else left_pressed = false;
	
	int c = mtype;
	
	for (int t = 0; t < env->world_dim*env->world_dim; t++){
		
		rect_manager->get_vertex(heat_ids[t], 0).heat = env->concentrations[c][t];
		rect_manager->get_vertex(heat_ids[t], 1).heat = env->concentrations[c][t];
		rect_manager->get_vertex(heat_ids[t], 2).heat = env->concentrations[c][t];
		rect_manager->get_vertex(heat_ids[t], 3).heat = env->concentrations[c][t];
		
	}
	env->simulation_step();
	cell_manager->simulation_step();
	cell_manager->render_step();
	
	window->clear_window();
	
	rect_manager->ready_for_draw();
	rect_manager->draw_all();
	
	cell_rect_manager->ready_for_draw();
	cell_rect_manager->draw_all();
	
	
	
	//draw
	window->update_frame();
	
}

void render_simulation(){
	
	return;
	
	
}