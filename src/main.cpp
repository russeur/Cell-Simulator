


#include "Window.hpp"
#include <ctime>
#include <fstream>
#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "MyGui.hpp"



#include <thread>


#include "CellManager.hpp"


struct heat_map_vert{
	
	GLfloat x, y;
	constexpr static GLfloat z = 0.0f;
	
	float heat;
	int is_wall;
	
};

RectangleID heat_ids[Environment::world_dim*Environment::world_dim];

void run_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager);
void run_paused_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager);

void render_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager, GLuint* simulation_FBO, GLuint* simulation_texture, Shader* texture_shader, VAO* texture_VAO, VBO* texture_VBO);


Position2D sim_size;

GLuint m_shaderProg;
GLuint BufSize;


GLuint m_posBuf[2];
int read_buff_idx = 0;


int main() {
	
	
	srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));srand(time(NULL));
	
	
	
	
	
	
	
	
	
	
	stbi_set_flip_vertically_on_load(true);

	
	Window* window = Window::init_window(900, 500, "Cells", "MAXIMIZED");
	window->set_clear_color(Color{0.0f, 0.0f, 0.0f, 1.0f}); // set background color
	
	
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window->get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
	
	{


	
	glEnable(GL_DEBUG_OUTPUT);
	/*glDebugMessageCallback(
	    [](GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar* msg, const void*) {
	        std::cout << "GL DEBUG: " << msg << std::endl;
	    },
	    nullptr
	);*/
	std::cout << "OpenGL version: "
          << glGetString(GL_VERSION) << std::endl;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	int maxCount[3], maxSize[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &maxSize[0]);
	
	std::cout<<"work groups count : "<<maxCount[0]<<", "<<maxCount[1]<<", "<<maxCount[2]<<" | work groups size"<<maxSize[0]<<", "<<maxSize[1]<<", "<<maxSize[2]<<"\n";
	
	//check for errors
	GLenum errorCode;
	if ((errorCode = glGetError()) != GL_NO_ERROR){
		std::string error;
		switch (errorCode){
			case GL_INVALID_ENUM: error = "INVALID ENUM"; break;
			case GL_INVALID_VALUE: error = "INVALID VALUE"; break;
			case GL_INVALID_OPERATION: error = "INVALID OPERATION"; break;
		}
		std::cout<<error<<"1\n";
	}else{
		std::cout<<"no error before\n";
	}



		
	//make envirnoment/simulation
	
	Environment env;
	
	env.set_all_concentrations(MoleculeConcentrations{0.5,0.0, 0.0, .1, 3.0, 3.0, 0.0, 0.0});
	
	Render_Rect_Manager<heat_map_vert> rect_manager("../src/shaders/heatmap_vert.txt", "../src/shaders/heatmap_frag.txt");
	
	rect_manager.start_init();
	rect_manager.link_attribute(1, GL_INT, 1, offsetof(heat_map_vert, heat));
	rect_manager.link_attribute(2, GL_INT, 1, offsetof(heat_map_vert, is_wall));
	rect_manager.finish_init();
	
	sim_size = Position2D{static_cast<GLfloat>(window->height), static_cast<GLfloat>(window->height)};//Position2D{928, 928};//
	std::cout<<sim_size.x<<": simsize\n";
	float visual_square_size = static_cast<float>(window->height) / static_cast<float>(env.world_dim);
	
	for (int t = 0; t < env.world_dim*env.world_dim; t++){
		
		int x_idx = t%env.world_dim;
		int y_idx = (t-x_idx) / env.world_dim;
		
		heat_ids[t] = rect_manager.new_rect(window->screen_to_UV_pos(Position2D{static_cast<float>(x_idx * visual_square_size), static_cast<float>(y_idx * visual_square_size)}, sim_size), window->screen_to_UV_vector(Position2D{static_cast<float>(visual_square_size), static_cast<float>(visual_square_size)}, sim_size), 0.0f, 0);
		
	}


	
	glCreateBuffers(2, m_posBuf);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_posBuf[0]);
	
	
	BufSize = (int)(env.world_dim*env.world_dim*(number_of_molecule_types+1)) * sizeof(float);

	//glBufferData(GL_SHADER_STORAGE_BUFFER, BufSize, nullptr, GL_DYNAMIC_DRAW);
	
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BufSize, env.concentrations, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_posBuf[1]);
	
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, BufSize, env.concentrations, GL_DYNAMIC_DRAW);
	//world.init(m_posBuf);
	
	
	
	
	std::string shaderString = get_file_contents("../src/shaders/particle_compute_shader.txt");
	const char* shaderSrc = shaderString.c_str();
	
	m_shaderProg = glCreateProgram();
	GLuint ShaderObj = glCreateShader(GL_COMPUTE_SHADER);
	
	if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", GL_COMPUTE_SHADER);
        return false;
    }
	
	glShaderSource(ShaderObj, 1, &shaderSrc, NULL);//attach shader txt and compile it
	glCompileShader(ShaderObj);
	GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling '%s': '%s'\n", "../src/shaders/particle_compute_shader.txt", InfoLog);
        return false;
    }
	glAttachShader(m_shaderProg, ShaderObj);//attach shader to shader program
	glLinkProgram(m_shaderProg);
	GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);

    if (Success == 0) {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_shaderProg);

    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);

    if (Success == 0) {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }
	//GLint attraction_matrix_location = glGetUniformLocation(m_shaderProg, "attraction_matrix");
	
	//GLuint m_velBuf;
	//GLuint m_vao;
	
	
	
	
	//glCreateBuffers(1, &m_velBuf);

    

    
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_velBuf);
    //glBufferData(GL_SHADER_STORAGE_BUFFER, BufSize, world.particle_vels, GL_DYNAMIC_DRAW);
	
	
	/*
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	
	glBindBuffer(GL_ARRAY_BUFFER, m_posBuf);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Position3D), (void*)0);
	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	
	
	
	glBindVertexArray(0);
	*/
	
	
	
	//check for errors
	
	if ((errorCode = glGetError()) != GL_NO_ERROR){
		std::string error;
		switch (errorCode){
			case GL_INVALID_ENUM: error = "INVALID ENUM"; break;
			case GL_INVALID_VALUE: error = "INVALID VALUE"; break;
			case GL_INVALID_OPERATION: error = "INVALID OPERATION"; break;
		}
		std::cout<<error<<"1\n";
	}else{
		std::cout<<"no error after either\n";
	}

	
	
	Render_Rect_Manager<CellVert> cell_rect_manager("../src/shaders/cell_vert.txt", "../src/shaders/cell_frag.txt");
	
	CellRenderManager cell_manager(&env, &cell_rect_manager, window);
	
	
	
	
	
	
	
	
	
	
	
	cell_rect_manager.start_init();
	cell_rect_manager.link_attribute(1, GL_FLOAT, 2, offsetof(CellVert, tx));
	cell_rect_manager.link_attribute(2, GL_FLOAT, 3, offsetof(CellVert, r));
	cell_rect_manager.finish_init();
	
	
	GLuint simulation_FBO;
	glGenFramebuffers(1, &simulation_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, simulation_FBO);
	
	GLuint simulation_texture;
	glGenTextures(1, &simulation_texture);
	glBindTexture(GL_TEXTURE_2D, simulation_texture);
	
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		sim_size.x,
		sim_size.y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr
	);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	
	
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		simulation_texture,
		0
	);
	
	// Check that it worked
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout<<"texture fro simulation failed!\n";
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	float vertices[] = {
		// position      // texture coordinate
		window->screen_to_UV_pos(Position2D{0, 0}).x, window->screen_to_UV_pos(Position2D{0, 0}).y,       0.0f, 1.0f,
		window->screen_to_UV_pos(Position2D{sim_size.x, 0}).x, window->screen_to_UV_pos(Position2D{0, 0}).y,       1.0f, 1.0f,
		window->screen_to_UV_pos(Position2D{sim_size.x, 0}).x, window->screen_to_UV_pos(Position2D{0, sim_size.y}).y,       1.0f, 0.0f,
	
		window->screen_to_UV_pos(Position2D{0, 0}).x, window->screen_to_UV_pos(Position2D{0, 0}).y,       0.0f, 1.0f,
		window->screen_to_UV_pos(Position2D{sim_size.x, 0}).x, window->screen_to_UV_pos(Position2D{0, sim_size.y}).y,       1.0f, 0.0f,
		window->screen_to_UV_pos(Position2D{0, 0}).x, window->screen_to_UV_pos(Position2D{0, sim_size.y}).y,       0.0f, 0.0f
	};
	
	
	Shader texture_shader("../src/shaders/single_texture_vert.txt", "../src/shaders/single_texture_frag.txt");
	VAO texture_VAO;
	VBO texture_VBO;
	
	
	texture_VAO.bind();
	texture_VBO.ready(&vertices, sizeof(vertices));
	
	texture_VAO.linkAttrib(texture_VBO, 0, GL_FLOAT, 2, 4 * sizeof(float), (void*)0);
	texture_VAO.linkAttrib(texture_VBO, 1, GL_FLOAT, 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	
	texture_VAO.unbind();
	texture_VBO.unbind();
	
	
	
	
	bool sim_is_paused = true;
	bool was_space_already_pressed = false;
	
	while(!window->window_should_close()){
		
		
		if (!was_space_already_pressed && (window->get_key_button_state(GLFW_KEY_SPACE) == GLFW_PRESS)){
			sim_is_paused = !sim_is_paused;
		}
		
		
		if (sim_is_paused){
			run_paused_simulation(window, &env, &rect_manager, &cell_rect_manager, &cell_manager);
			
		}else{
			run_simulation(window, &env, &rect_manager, &cell_rect_manager, &cell_manager);
			
		}
		was_space_already_pressed = (window->get_key_button_state(GLFW_KEY_SPACE) == GLFW_PRESS);
		render_simulation(window, &env, &rect_manager, &cell_rect_manager, &cell_manager, &simulation_FBO, &simulation_texture, &texture_shader, &texture_VAO, &texture_VBO);
		
		
		
	}
	}
	
	
	window->terminate();
	
	return 0;
}

int mtype = 0;
bool l_pressed = false;
bool r_pressed = false;
bool left_pressed = false;
bool vis = true;
float zoom = 1.0;
void run_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager){
	glBufferData(GL_SHADER_STORAGE_BUFFER, BufSize, env->concentrations, GL_DYNAMIC_DRAW);

	read_buff_idx = 1-read_buff_idx;
	
	if ((window->get_key_button_state(GLFW_KEY_0) == GLFW_PRESS)){
		vis = false;
	}
	if ((window->get_key_button_state(GLFW_KEY_1) == GLFW_PRESS)){
		vis = true;
		
	}
	
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
	
	
	
	if (l_pressed || r_pressed){
		std::cout<<mol_name[mtype]<<"\n";
	}
	
	
	
	int c = mtype;
	
	for (int t = 0; t < env->world_dim*env->world_dim; t++){
		
		rect_manager->get_vertex(heat_ids[t], 0).heat = env->concentrations[c*env->world_dim*env->world_dim+t];
		rect_manager->get_vertex(heat_ids[t], 1).heat = env->concentrations[c*env->world_dim*env->world_dim+t];
		rect_manager->get_vertex(heat_ids[t], 2).heat = env->concentrations[c*env->world_dim*env->world_dim+t];
		rect_manager->get_vertex(heat_ids[t], 3).heat = env->concentrations[c*env->world_dim*env->world_dim+t];


		
		rect_manager->get_vertex(heat_ids[t], 0).is_wall = env->concentrations[(number_of_molecule_types)*env->world_dim*env->world_dim+t];
		rect_manager->get_vertex(heat_ids[t], 1).is_wall = env->concentrations[(number_of_molecule_types)*env->world_dim*env->world_dim+t];
		rect_manager->get_vertex(heat_ids[t], 2).is_wall = env->concentrations[(number_of_molecule_types)*env->world_dim*env->world_dim+t];
		rect_manager->get_vertex(heat_ids[t], 3).is_wall = env->concentrations[(number_of_molecule_types)*env->world_dim*env->world_dim+t];
		
	}
	//std::thread t1([env]() {
    //	env->simulation_step();
	//});
	//std::thread t2([cell_manager]() {
    //	cell_manager->simulation_step();
	//});
	//
	glUseProgram(m_shaderProg);//choose what compute shader to use
		
	//glUniform1fv(attraction_matrix_location, 25, &(attraction_matrix[0]));
	
	//uniform time, like "glUniform3f(m_blackHole1Loc, Pos1.x, Pos1.y, Pos1.z);"
	GLenum errorCode;
	if ((errorCode = glGetError()) != GL_NO_ERROR){
		std::string error;
		switch (errorCode){
			case GL_INVALID_ENUM: error = "INVALID ENUM"; break;
			case GL_INVALID_VALUE: error = "INVALID VALUE"; break;
			case GL_INVALID_OPERATION: error = "INVALID OPERATION"; break;
		}
		std::cout<<c<<" << "<<error<<"3\n";
	}
	//std::cout<<"num_of_particles: "<<world.num_of_particles<<"\n";

	

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_posBuf[read_buff_idx]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_posBuf[1-read_buff_idx]);

	glDispatchCompute(env->world_dim*env->world_dim * number_of_molecule_types, 1, 1);// compute on a space of (num_of_particles(x), 1(y), 1(z))
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);// make sure things don't mess up this compute (idk what this does really, I justu know things can go wrong if not)
	

	//load data back into env
	
	
	
	cell_manager->simulation_step(m_shaderProg, BufSize, m_posBuf, read_buff_idx);
	//std::cout<<"start render\n";
	
	
}

Position2D zoom_offset = Position2D{0.0, 0.0};

std::weak_ptr<Cell> last_hover_cell;

void run_paused_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager){
	
	
	
	return;
}

Position2D mouse_start_press_pos;
bool click_was_in_sim = false;





void render_simulation(Window* window, Environment* env, Render_Rect_Manager<heat_map_vert>* rect_manager, Render_Rect_Manager<CellVert>* cell_rect_manager, CellRenderManager* cell_manager, GLuint* simulation_FBO, GLuint* simulation_texture, Shader* texture_shader, VAO* texture_VAO, VBO* texture_VBO){
	
	bool is_mouse_in_sim = window->cursor_x < sim_size.x && window->cursor_y < sim_size.y;
	if (is_mouse_in_sim){
		
		//float size_ratio = static_cast<float>(window->height) / static_cast<float>(env->world_dim*env->square_size);
		zoom += window->scroll * .1;
		if (zoom < 1.0) zoom = 1.0;
		window->scroll = 0;
	}
	
	std::weak_ptr<Cell> hovering_cell;
	for (int i = 0; i < static_cast<int>(cell_manager->cell_list.size()); i++){
		Cell* target_cell = cell_manager->cell_list[i].get();
		Position2D dist = target_cell->pos-Position2D{static_cast<GLfloat>(((window->cursor_x - zoom_offset.x)*(env->get_max_pos()/sim_size.x) - env->get_max_pos()/2.0)/zoom + env->get_max_pos()/2.0), static_cast<GLfloat>(((window->cursor_y - zoom_offset.y)*(env->get_max_pos()/sim_size.y) - env->get_max_pos()/2.0)/zoom + env->get_max_pos()/2.0)};
		if (std::sqrt((dist.x*dist.x) + (dist.y*dist.y)) < target_cell->size){
			hovering_cell = cell_manager->cell_list[i];
			break;
		}
	}
	
	
	
	last_hover_cell = hovering_cell;

	Position2D cursor_sim_pos = Position2D{static_cast<GLfloat>(((window->cursor_x - zoom_offset.x)*(env->get_max_pos()/sim_size.x) - env->get_max_pos()/2.0)/zoom + env->get_max_pos()/2.0), static_cast<GLfloat>(((window->cursor_y - zoom_offset.y)*(env->get_max_pos()/sim_size.y) - env->get_max_pos()/2.0)/zoom + env->get_max_pos()/2.0)};
	if (window->get_mouse_button_state(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		if (window->get_key_button_state(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			
			if (!left_pressed){
				
				if (MyGui::current_pointer_action == 4){
					Genetics::StartingDNA new_start(true);
					Genetics::RunningDNA new_run;
					
					cell_manager->add_cell(cursor_sim_pos, MoleculeConcentrations{7.0, 5.0, 0.0, 0.0, 5.0, 5.0}, new_start, new_run, -1);
				}
			}
			if (MyGui::current_pointer_action == 3){
				env->add_molecules_to_pos(cursor_sim_pos, MyGui::inject_concentrations);
			}else if (MyGui::current_pointer_action == 2){
				env->concentrations[number_of_molecule_types*env->world_dim*env->world_dim+env->get_tile_from_pos(cursor_sim_pos)] = 1;
				
			}else if (MyGui::current_pointer_action == 1){
				if (auto target_cell =hovering_cell.lock()){
					target_cell->admin_kill_cell();
				}
			}
		}else{
			if (!left_pressed){

				click_was_in_sim = is_mouse_in_sim;
				std::cout<<"max: "<<window->width<<", "<<window->height<<"\n";
				std::cout<<"max: "<<sim_size.x<<"\n";
				std::cout<<"pos: "<<window->cursor_x<<", "<<window->cursor_y<<"\n";
				if (click_was_in_sim){
					
					mouse_start_press_pos = Position2D{static_cast<GLfloat>(window->cursor_x), static_cast<GLfloat>(window->cursor_y)};
				}
			}else if (click_was_in_sim){
				zoom_offset +=  Position2D{static_cast<GLfloat>(window->cursor_x), static_cast<GLfloat>(window->cursor_y)} - mouse_start_press_pos;
				mouse_start_press_pos = Position2D{static_cast<GLfloat>(window->cursor_x), static_cast<GLfloat>(window->cursor_y)};
			}
		}

		

		//env->add_molecule_to_pos(Position2D{window->cursor_x, window->cursor_y}, static_cast<MoleculeType>(mtype), .1);
		//env->add_molecules_to_pos(Position2D{static_cast<float>(window->cursor_x), static_cast<float>(window->cursor_y)}/Position2D{size_ratio, size_ratio}, MoleculeConcentrations{-.2, .2, .2, 1.0});
		left_pressed = true;
	}else{
		if (left_pressed){
			if (mouse_start_press_pos == Position2D{static_cast<GLfloat>(window->cursor_x), static_cast<GLfloat>(window->cursor_y)}){
				if (auto hover_cell = last_hover_cell.lock()){
					MyGui::focused_cell = hover_cell;
					
				}else{
					hover_cell.reset();
				}
			}
		}
		left_pressed = false;
	}

	if (window->get_mouse_button_state(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
		if (window->get_key_button_state(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
			if (MyGui::current_pointer_action == 2){
				env->concentrations[number_of_molecule_types*env->world_dim*env->world_dim+env->get_tile_from_pos(cursor_sim_pos)] = 0;
				
			}
		}
	}


	if (vis){
	MyGui::start_frame();
	//ImGui::SetNextWindowPos(ImVec2(window->width-600, 0.0));
	
	MyGui::create_specie_menu(cell_manager);

	
	MyGui::create_cell_menu(env);

	MyGui::create_interaction_menu();

	
	
	
	}
	if (vis){
		
		cell_manager->render_step(MyGui::current_row_select);
		
		
		glBindFramebuffer(GL_FRAMEBUFFER, *simulation_FBO);
		glViewport(0, 0, sim_size.x, sim_size.y);
		window->set_clear_color(Color{0.0, 0.0, 0.3, 1.0});
		window->clear_window();
		
		
		
		rect_manager->ready_for_draw();
		cell_rect_manager->set_uniform("zoom", static_cast<GLfloat>(zoom));
		cell_rect_manager->set_uniform("zoom_offset", window->screen_to_UV_vector(Position2D{zoom_offset.x, zoom_offset.y}, sim_size).x, window->screen_to_UV_vector(Position2D{zoom_offset.x, zoom_offset.y}, sim_size).y);
		rect_manager->draw_all();
		
		
		cell_rect_manager->ready_for_draw();
		cell_rect_manager->set_uniform("zoom", static_cast<GLfloat>(zoom));
		cell_rect_manager->set_uniform("zoom_offset", window->screen_to_UV_vector(Position2D{zoom_offset.x, zoom_offset.y}, sim_size).x, window->screen_to_UV_vector(Position2D{zoom_offset.x, zoom_offset.y}, sim_size).y);
		cell_rect_manager->draw_all();
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window->width, window->height);
		window->set_clear_color(Color{0.0, 0.3, 0.0, 1.0});
		window->clear_window();
		
		texture_shader->activate();
		glUniform1i(glGetUniformLocation(texture_shader->ID, "tex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *simulation_texture);
		
		texture_VAO->bind();
		
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		texture_VAO->unbind();
		
		ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		
	}else{
		window->clear_window();
	}
	
	
	
	//draw
	window->update_frame();
	return;
	
	
}