#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "Camera.h"
#include "j1Input.h"

#define VSYNC true

j1Render::j1Render() : j1Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{
	for (std::vector<Camera*>::iterator item_cam = cameras.begin(); item_cam != cameras.end(); ++item_cam)
		{
			if ((*item_cam) != nullptr)
			{
				delete (*item_cam);
				(*item_cam) = nullptr;
			}
		}
		cameras.clear();
}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}
	
	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
	
		margin = config.child("margin").attribute("value").as_int();

		n_cameras_columns = config.child("n_cameras_columns").attribute("value").as_int();
		n_cameras_rows = config.child("n_cameras_rows").attribute("value").as_int();
		n_cameras_aux = config.child("n_cameras_aux").attribute("value").as_int();
		orientation = (ORIENTATION)config.child("orientation").attribute("value").as_int();

		CreateSplitScreen();

	}

	return ret;
}

void j1Render::CreateSplitScreen()
{

	uint final_width = 0;									//the final width that the camera will have in every case.
	uint final_height = 0;									//the final height that the camera will have in every case.
	uint n_cameras = 0;										//will be replaced by n_cameras_rows or n_cameras_columns depending on the orientation.
	uint n_cameras_max = 0;									//maximum number of cameras.


	uint n_cameras_columns_aux = 0;							//number of cameras in the last column. it is assigned the value of n_cameras_axu if the orientation is vertical automatically.
	uint n_cameras_rows_aux = 0;							//number of cameras in the last row. it is assigned the value of n_cameras_axu if the orientation is horizontal automatically.

	switch (orientation)
	{
	case ORIENTATION::NO_TYPE:
		LOG("the orientaiton NO_TYPE is not valid.");																	//don't have cameras aux. 
		break;
	case ORIENTATION::SQUARE_ORDER:
		//TODO 0: Calculate the max number of cameras in n_cameras_max with n_cameras_columns and n_cameras_rows									//Calcule the max number of cameras in this case.
		
		n_cameras_aux = 0;																		//don't have cameras aux. 
		break;
	case ORIENTATION::HORIZONTAL:
		n_cameras_columns_aux = n_cameras_aux;
		n_cameras_max = (n_cameras_rows - 1) * n_cameras_columns + n_cameras_columns_aux;		//Calcule the max number of cameras in this case. 
		break;
	case ORIENTATION::VERTICAL:
		n_cameras_rows_aux = n_cameras_aux;
		n_cameras_max = n_cameras_rows * (n_cameras_columns - 1) + n_cameras_rows_aux;			//Calcule the max number of cameras in this case.
		break;
	default:
		LOG("the orientaiton is not correct.");
		break;
	}

	/* The + 1 is because there will always be one more margin */
	float width = (App->win->screen_surface->w - ((n_cameras_columns + 1) * margin)) / n_cameras_columns;				//screen width - the sum of all margin (width) / number of columns
	float width_aux = (App->win->screen_surface->w - ((n_cameras_columns_aux + 1) * margin)) / n_cameras_columns_aux;	//the same but with differents number of columns

	float height = (App->win->screen_surface->h - ((n_cameras_rows + 1)*margin)) / n_cameras_rows;						//screen height - the sum of all margin (height) / number of rows
	float height_aux = (App->win->screen_surface->h - ((n_cameras_rows_aux + 1)*margin)) / n_cameras_rows_aux;			//the same but with differents number of rows


	for (uint i = 0; i < n_cameras_max; ++i)
	{
		Camera* camera_aux = nullptr;
		camera_aux = new Camera();

		if (orientation == ORIENTATION::HORIZONTAL || orientation == ORIENTATION::SQUARE_ORDER)							//it is the same for the case of horizontal orientation as the case of square orientation, but the square will not enter in the else. 
		{
			final_height = height;																						//assign the height, is the same in all the positions.
			n_cameras = n_cameras_columns;

			if (n_cameras_max - i > n_cameras_columns_aux)																//if the camera is not in the last row.
			{
				final_width = width;																					//assign the normal width.

				//TODO 1: Calculate the position of every camera in the screen in camera_aux->screen_section.
				
			
			
			}
			else																										//if the camera is in the last row. the square orientation will not enter here.
			{
				final_width = width_aux;																				//assign the width_aux in the last row becuase is different.
				n_cameras_aux = n_cameras_columns_aux;
				//TODO 4: Calculate the position of every camera in the screen in camera_aux->screen_section.
				//remember that now the number of cameras is not the same.
				


			}
		}
		else if (orientation == ORIENTATION::VERTICAL)
		{
			final_width = width;																						//assign the width, is the same in all the positions.
			n_cameras = n_cameras_rows;

			if (n_cameras_max - i > n_cameras_rows_aux)																	//if the camera is not in the last column.
			{
				final_height = height;																					//assign the normal height.
				//TODO 5: Calculate the position of every camera in the screen in camera_aux->screen_section.
				//now we do not count from left to right, now we count from top to bottom.
				
			
			
			}
			else
			{
				final_height = height_aux;																				//assign the height_aux in the last column becuase is different.
				n_cameras_aux = n_cameras_rows_aux;
				//TODO 5: Calculate the position of every camera in the screen in camera_aux->screen_section.
				//In the last column it happens the same, we don’t have the same number of cameras.
				
			
			}
		}

		camera_aux->rect.w = camera_aux->screen_section.w = final_width;									//assign the final width in the rect and the screen_section defined above in each case.
		camera_aux->rect.h = camera_aux->screen_section.h = final_height;									//assign the final height in the rect and the screen_section definded above in each case.

		cameras.push_back(camera_aux);
	}
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);

	std::vector<Camera*>::iterator item_cam;
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		debug = !debug;
		if (!debug)
		{
			for (uint i = cameras.size(); i > 1; i = cameras.size())
			{
				camera_saves.push_back(cameras.back());
				cameras.pop_back();
			}

			width_of_first_camera = cameras.front()->rect.w;
			height_of_first_camera = cameras.front()->rect.h;

			cameras.front()->rect.w = cameras.front()->screen_section.w = App->win->screen_surface->w - margin * 2;
			cameras.front()->rect.h = cameras.front()->screen_section.h = App->win->screen_surface->h - margin * 2;

		}
		else
		{
			for (uint i = camera_saves.size(); i > 0; i = camera_saves.size())
			{
				cameras.push_back(camera_saves.back());
				camera_saves.pop_back();
			}

			cameras.front()->rect.w = cameras.front()->screen_section.w = width_of_first_camera;
			cameras.front()->rect.h = cameras.front()->screen_section.h = height_of_first_camera;
			
		}
	}

	else if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->win->SetScale(1.f);
	}
	else if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->win->SetScale(0.5f);
	}
	else if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		App->win->SetScale(2.f);
	}
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

// Blit to screen
void j1Render::Blit(SDL_Texture* texture, const int screen_x, const int screen_y, Camera* current_camera, const SDL_Rect* section) const
{
	float scale = App->win->GetScale();

	SDL_Rect rect_in_screen{ 0,0,0,0 };

	//TODO 2: Calculate the new position in rect_in_screen. Remember that you calculate the position on screen in camera_aux->screen_section.
	rect_in_screen.x = -current_camera->rect.x + screen_x * scale;
	rect_in_screen.y = -current_camera->rect.y + screen_y * scale;
	


	if (section != NULL)
	{

		rect_in_screen.w = section->w;
		rect_in_screen.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect_in_screen.w, &rect_in_screen.h);
	}

	rect_in_screen.w *= scale;
	rect_in_screen.h *= scale;

	

	//Print the rect_in_screen ============================================
	if (SDL_RenderCopy(renderer, texture, section, &rect_in_screen))
	{
		LOG("Cannot blit to main_object. SDL_RenderCopy error: %s", SDL_GetError());
	}
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	float scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, -cameras[0]->rect.x + x1 * scale, -cameras[0]->rect.y + y1 * scale, -cameras[0]->rect.x + x2 * scale, -cameras[0]->rect.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::IsOnCamera(const int & x, const int & y, const int & w, const int & h, Camera* camera) const
{
	float scale = App->win->GetScale();

	SDL_Rect r = { x*scale,y*scale,w*scale,h*scale };

	return SDL_HasIntersection(&r, &camera->rect);
}
