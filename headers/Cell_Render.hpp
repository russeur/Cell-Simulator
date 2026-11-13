#include "Cell.hpp"
#include "RectManagerClass.hpp"
#include "Window.hpp"

struct CellVert{
	GLfloat x, y;
	constexpr static GLfloat z = 0.0f;
	
	GLfloat tx, ty;
	
	GLfloat r, g, b;
	
};


class Cell_Render: public Cell{
	
	void notify_add_connection() override;
	void notify_remove_connection(int i) override;
	
	Render_Rect_Manager<CellVert>* render_rect_man;
	Window* window;
	
	RectangleID rect_id;
	RectangleID angle_notifier_id;
	
	std::vector<RectangleID> adhesion_markers;
	
	Position2D sim_size;
	Position2D sim_ratio;
	
	public:
	
	
	void render_step(int selected_species_id);
	
	
	
	Cell_Render(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations, Render_Rect_Manager<CellVert>*, Window*);
	Cell_Render(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations, const Genetics::StartingDNA& parent_start_dna, const Genetics::RunningDNA& parent_run_dna, int specie_parent_index, Render_Rect_Manager<CellVert>*, Window*);
	~Cell_Render();
	
	
};

