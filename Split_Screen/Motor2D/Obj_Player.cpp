#include "SDL/include/SDL.h"

#include "j1App.h"
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

int Obj_Player::number_of_tanks = 0;

Obj_Player::Obj_Player(fPoint pos) : Object(pos)
{
	tank_num = number_of_tanks++;
}

Obj_Player::~Obj_Player()
{
	/*number_of_tanks--;
	if (app->on_clean_up == false)
	{
		if (tutorial_move != nullptr)
		{
			tutorial_move->Destroy();
		}
		if (tutorial_pick_up != nullptr)
		{
			tutorial_pick_up->Destroy();
		}
		if (tutorial_revive != nullptr)
		{
			tutorial_revive->Destroy();
		}
	}

	if (camera_player != nullptr)
	{
		camera_player->assigned = false;
		camera_player = nullptr;
	}*/
}

bool Obj_Player::Start()
{
	SDL_Texture* tex_player_a = App->tex->Load("textures/Player_A.png");
	switch (tank_num) {
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
		curr_tex = tex_player_a;

		break;
	case 2:
		kb_up		= SDL_SCANCODE_I;
		kb_left		= SDL_SCANCODE_J;
		kb_down		= SDL_SCANCODE_K;
		kb_right	= SDL_SCANCODE_L;
		curr_tex = tex_player_a;

		break;
	case 3:
		kb_up		= SDL_SCANCODE_KP_8;
		kb_left		= SDL_SCANCODE_KP_4;
		kb_down		= SDL_SCANCODE_KP_5;
		kb_right	= SDL_SCANCODE_KP_6;
		curr_tex = tex_player_a;

		break;
	default:
		LOG("Number of tanks is greater than 3. You probably restarted the game and need to set the variable to 0 again.");
		break;
	}

	speed = 5.f;//TODO: Load from xml

	cos_45 = cosf(-45 * DEGTORAD);
	sin_45 = sinf(-45 * DEGTORAD);

	
	std::vector<Camera*>::iterator item_cam;

	for (item_cam = App->render->cameras.begin(); item_cam != App->render->cameras.end(); ++item_cam)
	{
		if (!(*item_cam)->assigned)
		{
			(*item_cam)->assigned = true;
			camera_player = (*item_cam);
			break;
		}
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

	//camera_player->ResetCamera();
	//camera_player->FollowPlayer(dt, this);
	//camera_player->ShakeCamera(dt);
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

	/*if (!iso_dir.IsZero())
	{

		float target_angle = atan2(input_dir.y, -input_dir.x) * RADTODEG;
		//Calculate how many turns has the base angle and apply them to the target angle
		float turns = floor(angle / 360.f);
		target_angle += 360.f * turns;
		//Check which distance is shorter. Rotating clockwise or counter-clockwise
		if (abs((target_angle + 360.f) - angle) < abs(target_angle - angle))
		{
			target_angle += 360.f;
		}
		angle = lerp(angle, target_angle, base_angle_lerp_factor * dt);

	}*/

	velocity = iso_dir * speed * dt;


	pos_map += velocity;

}


void Obj_Player::InputMovementKeyboard(fPoint & input)
{
	if (App->input->GetKey(kb_up) == KEY_DOWN || App->input->GetKey(kb_up) == KEY_REPEAT)
	{
		//app->render->camera.y -= floor(100.0f * dt);
		input.y -= 1.f;
	}
	if (App->input->GetKey(kb_left) == KEY_DOWN || App->input->GetKey(kb_left) == KEY_REPEAT)
	{
		//app->render->camera.x -= floor(100.0f * dt);
		input.x -= 1.f;
	}
	if (App->input->GetKey(kb_down) == KEY_DOWN || App->input->GetKey(kb_down) == KEY_REPEAT)
	{
		//app->render->camera.y += floor(100.0f * dt);
		input.y += 1.f;
	}
	if (App->input->GetKey(kb_right) == KEY_DOWN || App->input->GetKey(kb_right) == KEY_REPEAT)
	{
		//app->render->camera.x += floor(100.0f * dt);
		input.x += 1.f;
	}


}


//bool Obj_Player::Draw(float dt, Camera * camera)
//{
//	// Base =========================================
//	app->render->Blit(
//		curr_tex,
//		pos_screen.x - draw_offset.x,
//		pos_screen.y - draw_offset.y,
//		camera,
//		&curr_anim->GetFrame(angle));
//
//	if (show_crosshairs && camera == camera_player)
//	{
//		float line_length = 5.f;
//		//1-- Set a position in the isometric space
//		fPoint input_iso_pos(turr_pos.x + shot_dir.x * line_length, turr_pos.y + shot_dir.y * line_length);
//		//2-- Transform that poin to screen coordinates
//		iPoint input_screen_pos = (iPoint)app->map->MapToScreenF(input_iso_pos);
//		app->render->DrawLineSplitScreen(
//			pos_screen.x, pos_screen.y - cannon_height,
//			input_screen_pos.x, input_screen_pos.y, 0, 0, 255, 123, camera);
//	}
//
//	// Turret =======================================
//	app->render->Blit(
//		turr_tex,
//		pos_screen.x - draw_offset.x,
//		pos_screen.y - draw_offset.y,
//		camera,
//		&rotate_turr.GetFrame(turr_angle));
//
//																							//only appears when hes dead and disappear when he has been revived
//	//DEBUG
//	{
//		//	iPoint debug_mouse_pos = { 0, 0 };
////	app->input->GetMousePosition(debug_mouse_pos.x, debug_mouse_pos.y);
//
////	debug_mouse_pos.x += camera_player->rect.x;
////	debug_mouse_pos.y += camera_player->rect.y;
//
////	fPoint shot_pos(pos_map - app->map->ScreenToMapF( 0.f, cannon_height ));
////	fPoint debug_screen_pos = app->map->MapToScreenF(shot_pos);
//
//	//  std::vector<Camera*>::iterator item_cam;
////	for (item_cam = app->render->camera.begin(); item_cam != app->render->camera.end(); ++item_cam)
////	{
//	//	app->render->DrawLineSplitScreen((*item_cam), debug_mouse_pos.x, debug_mouse_pos.y, debug_screen_pos.x, debug_screen_pos.y,  0, 255, 0);
////	}
//	}
//
//
//
//	return true;
//}


bool Obj_Player::CleanUp()
{
	return true;
}
