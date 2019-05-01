#include "Camera.h"
#include "MathUtils.h"

Camera::Camera()
{
	lerp_factor = 5.f;
}


void Camera::FollowPlayer(float dt, Obj_Player * player)
{
	fPoint source_pos((float) rect.x, (float)rect.y);
	fPoint target_pos (player->pos_screen.x - rect.w * 0.5f, player->pos_screen.y - rect.h * 0.5f);

	fPoint lerp_pos = lerp(source_pos, target_pos, dt * lerp_factor);

	rect.x = lerp_pos.x;
	rect.y = lerp_pos.y;
}



