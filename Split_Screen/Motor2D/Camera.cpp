#include "Camera.h"
#include "MathUtils.h"
#include "j1Window.h"
#include "App.h"

Camera::Camera()
{
	lerp_factor = 5.f;
}


void Camera::FollowPlayer(float dt, Obj_Player * player)
{
	fPoint source_pos((float) rect.x, (float)rect.y);
	fPoint target_pos (player->pos_screen.x * App->win->GetScale() - rect.w * 0.5f, player->pos_screen.y* App->win->GetScale() - rect.h * 0.5f);

	fPoint lerp_pos = lerp(source_pos, target_pos, dt * lerp_factor);

	rect.x = lerp_pos.x;
	rect.y = lerp_pos.y;
}



