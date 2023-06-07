#include "MushRoom.h"
#include "Utils.h"

CMushRoom::CMushRoom(int nx, int color)
{
	this->color = color;
	StartRising();
	vx = MUSHROOM_MOVING_SPEED * nx;
}

void CMushRoom::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x + 3.0f;
	t = y;
	r = l + MUSHROOM_BBOX_WIDTH - 6.0f;
	b = y + MUSHROOM_BBOX_HEIGHT;
}

void CMushRoom::Render()
{
	if (isFinish)
		return;
	int ani = MUSHROOM_ANI_RED;
	if (color == 1)
		ani = MUSHROOM_ANI_GREEN;	
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CMushRoom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish)
		return;
	CGameObject::Update(dt);


	if (GetTickCount64() - rise_start > MUSHROOM_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
	}

	if (rising)
		y -= 0.5;
	else
		state = MUSHROOM_STATE_MOVING;

	if (state == MUSHROOM_STATE_MOVING || state == MUSHROOM_STATE_FLY)	// Out of QBrick
	{

		vy += MUSHROOM_GRAVITY * dt;

		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();

		CalcPotentialCollisions(coObjects, coEvents);

		if (coEvents.size() == 0)
		{
			x += dx;
			y += dy;
		}
		else
		{

			float min_tx, min_ty, nx = 0, ny;
			float rdx = 0;
			float rdy = 0;

			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

			x += min_tx * dx + nx * 0.4f;
			//y += min_ty * dy + ny * 0.4f;

			if (ny != 0) vy = 0;

			for (int i = 0; i < int(coEventsResult.size()); i++)
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
			}

		}
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	}
}

void CMushRoom::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case MUSHROOM_STATE_MOVING:
		vx = -MUSHROOM_MOVING_SPEED;
		break;
	case MUSHROOM_STATE_FLY:
		vy = -MUSHROOM_FLY_SPEED;
		vx = -MUSHROOM_MOVING_SPEED;
		break;
	}
}
