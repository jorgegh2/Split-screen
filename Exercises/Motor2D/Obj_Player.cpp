#include "SDL/include/SDL.h"

#include "App.h"
#include "Obj_Player.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1ObjManager.h"
#include "j1Window.h"
#include "Camera.h"
#include "Defs.h"
#include "Log.h"

int Obj_Player::number_of_players = 0;

Obj_Player::Obj_Player(fPoint pos) : Object(pos)
{
	player_num = number_of_players++;
}

Obj_Player::~Obj_Player()
{
	number_of_players--;

	if (camera_player != nullptr)
	{
		camera_player->assigned = false;
		camera_player = nullptr;
	}
}

bool Obj_Player::Start()
{
	SDL_Texture* tex_player_a = App->tex->Load("textures/Player_A.png");
	SDL_Texture* tex_player_b = App->tex->Load("textures/Player_B.png");
	SDL_Texture* tex_player_c = App->tex->Load("textures/Player_C.png");
	SDL_Texture* tex_player_d = App->tex->Load("textures/Player_D.png");

	switch (player_num) {
	case 0:
		kb_up		= SDL_SCANCODE_W;
		kb_left		= SDL_SCANCODE_A;
		kb_down		= SDL_SCANCODE_S;
		kb_right	= SDL_SCANCODE_D;
		curr_tex = tex_player_a;
		break;
	case 1:
		kb_up		= SDL_SCANCODE_T;
		kb_left		= SDL_SCANCODE_F;
		kb_down		= SDL_SCANCODE_G;
		kb_right	= SDL_SCANCODE_H;
		curr_tex = tex_player_b;

		break;
	case 2:
		kb_up		= SDL_SCANCODE_I;
		kb_left		= SDL_SCANCODE_J;
		kb_down		= SDL_SCANCODE_K;
		kb_right	= SDL_SCANCODE_L;
		curr_tex = tex_player_c;

		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		curr_tex = tex_player_d;

		break;
	default:
		LOG("Number of tanks is greater than 3. You probably restarted the game and need to set the variable to 0 again.");
		break;
	}

	speed = 5.f;

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	
	//TODO 3: Assign one camera that is not assigned yet to the current player. When we assign one camera, we don't want to continue in the loop.
	for (std::vector<Camera*>::iterator item_cam = App->render->cameras.begin(); item_cam != App->render->cameras.end(); ++item_cam)
	{
	
		
	}
	return true;
}

bool Obj_Player::PreUpdate()
{
	return true;
}

bool Obj_Player::Update(float dt)
{
	Movement(dt);
	CameraMovement(dt);//Camera moves after the player

	return true;
}

void Obj_Player::CameraMovement(float dt)
{
	if (camera_player == nullptr)
		return;

	camera_player->FollowPlayer(dt, this);
}

void Obj_Player::Movement(float dt)
{
	fPoint input_dir(0.f, 0.f);


	InputMovementKeyboard(input_dir);
	
	//The tank has to go up in isometric space, so we need to rotate the input vector by 45 degrees
	fPoint iso_dir(0.f, 0.f);
	iso_dir.x = input_dir.x * cos_45 - input_dir.y * sin_45;
	iso_dir.y = input_dir.x * sin_45 + input_dir.y * cos_45;
	iso_dir.Normalize();

	velocity = iso_dir * speed * dt;


	pos_map += velocity;

}


void Obj_Player::InputMovementKeyboard(fPoint & input)
{
	if (App->input->GetKey(kb_up) == KEY_DOWN || App->input->GetKey(kb_up) == KEY_REPEAT)
	{
		input.y -= 1.f;
	}
	if (App->input->GetKey(kb_left) == KEY_DOWN || App->input->GetKey(kb_left) == KEY_REPEAT)
	{
		input.x -= 1.f;
	}
	if (App->input->GetKey(kb_down) == KEY_DOWN || App->input->GetKey(kb_down) == KEY_REPEAT)
	{
		input.y += 1.f;
	}
	if (App->input->GetKey(kb_right) == KEY_DOWN || App->input->GetKey(kb_right) == KEY_REPEAT)
	{
		input.x += 1.f;
	}


}


bool Obj_Player::CleanUp()
{
	return true;
}
