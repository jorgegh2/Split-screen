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
		Camera* camera_aux1 = nullptr;
		camera_aux1 = new Camera();

		Camera* camera_aux2 = nullptr;
		camera_aux2 = new Camera();

		Camera* camera_aux3 = nullptr;
		camera_aux3 = new Camera();

		Camera* camera_aux4 = nullptr;
		camera_aux4 = new Camera();

		camera_aux1->rect.w = App->win->screen_surface->w * .5f;
		camera_aux2->rect.w = App->win->screen_surface->w * .5f;
		camera_aux3->rect.w = App->win->screen_surface->w * .5f;
		camera_aux4->rect.w = App->win->screen_surface->w * .5f;

		camera_aux1->rect.h = App->win->screen_surface->h * .5f;
		camera_aux2->rect.h = App->win->screen_surface->h * .5f;
		camera_aux3->rect.h = App->win->screen_surface->h * .5f;
		camera_aux4->rect.h = App->win->screen_surface->h * .5f;

		camera_aux1->screen_section = {
			0,
			0,
			(int)(App->win->screen_surface->w * .5f),
			(int)(App->win->screen_surface->h * .5f)
		};

		camera_aux2->screen_section = {
			(int)(App->win->screen_surface->w * .5f),
			0,
			(int)(App->win->screen_surface->w * .5f),
			(int)(App->win->screen_surface->h * .5f)
		};

		camera_aux3->screen_section = {
			0,
			(int)(App->win->screen_surface->h * .5f),
			(int)(App->win->screen_surface->w * .5f),
			(int)(App->win->screen_surface->h * .5f)
		};

		camera_aux4->screen_section = {
		(int)(App->win->screen_surface->w * .5f),
		(int)(App->win->screen_surface->h * .5f),
		(int)(App->win->screen_surface->w * .5f),
		(int)(App->win->screen_surface->h * .5f)
		};

		cameras.push_back(camera_aux1);
		cameras.push_back(camera_aux2);
		cameras.push_back(camera_aux3);
		cameras.push_back(camera_aux4);

	}

	return ret;
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
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		debug = !debug;
		if (!debug)
		{
			camera_saves.push_back(cameras.back());
			cameras.pop_back();

			camera_saves.push_back(cameras.back());
			cameras.pop_back();

			camera_saves.push_back(cameras.back());
			cameras.pop_back();

			cameras.front()->rect.w = App->win->screen_surface->w;
			cameras.front()->rect.h = App->win->screen_surface->h;

			cameras.front()->screen_section.w = App->win->screen_surface->w;
			cameras.front()->screen_section.h = App->win->screen_surface->h;
		}
		else
		{
			cameras.push_back(camera_saves.back());
			camera_saves.pop_back();

			cameras.push_back(camera_saves.back());
			camera_saves.pop_back();

			cameras.push_back(camera_saves.back());
			camera_saves.pop_back();

			cameras.front()->rect.w *= 0.5f;
			cameras.front()->rect.h *= 0.5f;

			cameras.front()->screen_section.w *= 0.5f;
			cameras.front()->screen_section.h *= 0.5f;
		}
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

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	//camera.x = data.child("camera").attribute("x").as_int();
	//camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	/*cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;*/

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
void j1Render::Blit(SDL_Texture* texture, const int screen_x, const int screen_y, Camera* current_camera, const SDL_Rect* section) const
{
	uint scale = App->win->GetScale();

	SDL_Rect rect_in_screen;
	SDL_Rect spritesheet_rect{ 0,0,0,0 };

	//Transform the rect in the word to the rect in screen =======================
	rect_in_screen.x = -current_camera->rect.x + screen_x * scale;
	rect_in_screen.y = -current_camera->rect.y + screen_y * scale;

	if (section != NULL)
	{
		spritesheet_rect = *section;
		rect_in_screen.w = section->w * scale;
		rect_in_screen.h = section->h * scale;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect_in_screen.w, &rect_in_screen.h);
		spritesheet_rect.w = rect_in_screen.w;
		spritesheet_rect.h = rect_in_screen.h;
	}

	//Move the rect_in_screen to their correct screen =========================== 	
	rect_in_screen.x += current_camera->screen_section.x;
	rect_in_screen.y += current_camera->screen_section.y;

	//Print the rect_in_screen ============================================
	if (SDL_RenderCopy(renderer, texture, &spritesheet_rect, &rect_in_screen))
	{
		LOG("Cannot blit to main_object. SDL_RenderCopy error: %s", SDL_GetError());
	}
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

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
	int scale = App->win->GetScale();

	SDL_Rect r = { x*scale,y*scale,w*scale,h*scale };

	return SDL_HasIntersection(&r, &camera->rect);
}
