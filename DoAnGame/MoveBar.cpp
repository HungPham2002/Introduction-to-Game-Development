#include "MoveBar.h"
#include "Brick.h"
#include "Utils.h"
CMoveBar::CMoveBar()
{
	SetState(BAR_STATE_MOVING);
}

void CMoveBar::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + BAR_BBOX_WIDTH;
	bottom = y + BAR_BBOX_HEIGHT;
}

void CMoveBar::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);
	if ( x > camx + scrw)		// out screen width then return
		return;

	if (x + BAR_BBOX_WIDTH < camx || y > camy + scrh)
	{
		isFinish = 1;
		isStomped = 0;
	}


	CGameObject::Update(dt);


	if (state != BAR_STATE_MOVING)	
		vy += BAR_GRAVITY * dt;

	x += dx;
	y += dy;
}

void CMoveBar::Render()
{
	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CMoveBar::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BAR_STATE_MOVING:
		vx = -BAR_MOVING_SPEED;
		vy = 0;
		break;
	case BAR_STATE_FALLING:
		vx = 0;
		break;
	}
}
