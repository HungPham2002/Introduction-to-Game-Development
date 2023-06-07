#include "Fragment.h"


CFragment::CFragment(int side)
{
	//SetState(FIREBALL_STATE_SPIN);
	ny = -1;
	if (side == 0)							// fragment right tall
	{
		nx = 1;
		vx = FRAGMENT_SPIN_SPEED_VX * nx;
		vy = FRAGMENT_SPIN_SPEED_VY * ny;
	}
	else if (side == 1)						// fragment right short
	{
		nx = 1;
		vx = FRAGMENT_SPIN_SPEED_VX * nx;
		vy = FRAGMENT_SPIN_SPEED_VY1 * ny;

	}
	else if (side == 2)						// fragment left tall
	{
		nx = -1;
		vx = FRAGMENT_SPIN_SPEED_VX * nx;
		vy = FRAGMENT_SPIN_SPEED_VY * ny;
	}
	else if (side == 3)						// fragment left short
	{
		nx = -1;
		vx = FRAGMENT_SPIN_SPEED_VX * nx;	
		vy = FRAGMENT_SPIN_SPEED_VY1 * ny;
	}


}

void CFragment::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}

void CFragment::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);

	if (x < camx || x > camx + scrw)						// delete fireball if out map
		isFinish = 1;
	if (y > camy + scrh)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);

	vy += FRAGMENT_GRAVITY * dt;

	x += dx;
	y += dy;

}

void CFragment::Render()
{
	if (isFinish)
		return;
	animation_set->at(0)->Render(round(x), round(y));
}

