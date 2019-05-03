#ifndef __OBJ_TANK_H__
#define __OBJ_TANK_H__

#include "Object.h"
#include "j1Input.h"
#include "Point.h"


struct SDL_Texture;
class Camera;


class Obj_Player : public Object
{
public:
	Obj_Player(fPoint pos);
	~Obj_Player();

public:

	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;


	bool CleanUp() override;


private:
	//- Movement
	void Movement(float dt);
	void InputMovementKeyboard(fPoint & input);

	//- Camera
	void CameraMovement(float dt);


private:

	//- Logic
	int player_num							= 0;//The number of tank. 0 is the first one.

	static int number_of_players;

	float cos_45 = 0.f;
	float sin_45 = 0.f;

	//- Movement
	float speed								= 0.f;

	SDL_Scancode kb_up						= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_left					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_down					= SDL_SCANCODE_UNKNOWN;
	SDL_Scancode kb_right					= SDL_SCANCODE_UNKNOWN;


public:
	Camera* camera_player				= nullptr;
};

#endif