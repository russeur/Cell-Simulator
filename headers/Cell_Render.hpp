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
	
	public:
	
	
	void render_step();
	
	
	
	Cell_Render(Environment*, CellRenderManager*, Position2D, MoleculeConcentrations, Render_Rect_Manager<CellVert>*, Window*);
	
	
	
};

