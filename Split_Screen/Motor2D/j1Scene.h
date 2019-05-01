#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "Obj_Player.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
	SDL_Texture* debug_tex;
	Obj_Player* player_A = nullptr;
	Obj_Player* player_B = nullptr;
	Obj_Player* player_C = nullptr;
	Obj_Player* player_D = nullptr;
	Obj_Player* player_E = nullptr;
	Obj_Player* player_F = nullptr;
};

#endif // __j1SCENE_H__