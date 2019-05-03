#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "j1Module.h"
#include <vector>

enum class ORIENTATION
{
	NO_TYPE = 0,
	SQUARE_ORDER,			//All cameras have the same width and height, even if it is not square order the maximum number of cameras.
	HORIZONTAL,				//The cameras aux will have more width than the rest to occupy the whole row.
	VERTICAL,				//The cameras aux will have more height than the rest to occupy the whole column.
};


class Camera;
class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	void CreateSplitScreen();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Draw & Blit
	void Blit(SDL_Texture* texture, int screen_x, int screen_y, Camera* camera, const SDL_Rect* section = NULL) const;
	
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	bool IsOnCamera(const int & x, const int & y, const int & w, const int & h, Camera* camera) const;


	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:

	SDL_Renderer*					renderer = nullptr;
	std::vector<Camera*>			cameras;
	std::vector<Camera*>			camera_saves;
	SDL_Rect						viewport;
	SDL_Color						background;

	uint							width_of_first_camera = 0;
	uint							height_of_first_camera = 0;

	float							margin = 0;								//size of margin.							
	uint							n_cameras_columns = 0;					//number of columns.
	uint							n_cameras_rows = 0;						//number of rows.
	uint							n_cameras_aux = 0;						//number of cameras in the last row or column (regardless of the orientation, selected if its rows or columns in the orientation).
	ORIENTATION						orientation	= ORIENTATION::NO_TYPE;		//orientation of the cameras, look the declaration for more information.

	bool debug = true;
};

#endif // __j1RENDER_H__