#include "Boomerang.h"
#include "Utils.h"


CBoomerang::CBoomerang(float y)
{
	originY = y;
	ny = 1;
	nx = 1;
	vx = BOOMERANG_SPIN_SPEED_VX * nx;
	vy = BOOMERANG_SPIN_SPEED_VY * ny;
}

void CBoomerang::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + BOOMERANG_BBOX_WIDTH;
	b = y + BOOMERANG_BBOX_HEIGHT;
}

void CBoomerang::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);

	if (x < camx || x > camx + scrw)						// delete boomerang if out map
		isFinish = 1;
	if (y < camy || y > camy + scrh)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);

	if (this->y < originY - 30.0f)					// reach max height then fall
	{
		vx = 0;
		vy += BOOMERANG_GRAVITY_Y * dt;
	}
	if (this->y > originY + 15.0f && vy > 0)		//	reach min height then back to boomerangbros
	{
		vy = 0;
		vx += -BOOMERANG_GRAVITY_X * dt;
	}

	x += dx;
	y += dy;

}

void CBoomerang::Render()
{
	if (isFinish)
		return;
	animation_set->at(0)->Render(round(x), round(y));
}

