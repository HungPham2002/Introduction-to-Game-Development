#include "Leaf.h"
#include "Utils.h"

CLeaf::CLeaf(float x)
{
	vx = LEAF_FALL_SPEED_VX;
	vy = LEAF_FALL_SPEED_VY;
	xMax = x + LEAF_BBOX_WIDTH * 2;
	xMin = x - 1;
	StartRising();
}

void CLeaf::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)	//turn of boundingbox
		return;
	l = x;
	t = y;
	r = x + LEAF_BBOX_WIDTH;
	b = y + LEAF_BBOX_HEIGHT;
}

void CLeaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);

	if (x < camx || x > camx + scrw)						// delete if out map
		isFinish = 1;
	if (y < camy || y > camy + scrh - BOARD_HEIGHT)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);

	if (GetTickCount64() - rise_start > LEAF_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
	}

	if (rising)
		y -= 1.0f;
	else
		state = LEAF_STATE_FALLING;

	if (state == LEAF_STATE_FALLING)
	{
		x += dx;
		y += dy;
	}
	if (x > xMax || x < xMin)
		vx = -vx;
}

void CLeaf::Render()
{
	if (isFinish)
		return;
	int ani = LEAF_FALL_RIGHT;
	if (vx >= 0) ani = LEAF_FALL_LEFT;
	animation_set->at(ani)->Render(round(x), round(y));
}


void CLeaf::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case LEAF_STATE_FALLING:
		break;
	}
}