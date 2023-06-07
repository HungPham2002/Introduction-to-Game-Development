#include <algorithm>
#include <assert.h>
#include "Utils.h"

#include "MiniMario.h"
#include "Game.h"



CMiniMario::CMiniMario(float x, float y) : CGameObject()
{
	level = MINIMARIO_LEVEL_SMALL;
	SetState(MINIMARIO_STATE_IDLE);

	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;
}

void CMiniMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	// Calculate dx, dy 
	CGameObject::Update(dt, coObjects);


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	CalcPotentialCollisions(coObjects, coEvents);

	if (GetState() == MINIMARIO_STATE_WALKING_RIGHT)
		if (this->x >= tempLocationX)
		{
			this->x = tempLocationX;
			SetState(MINIMARIO_STATE_IDLE);
		}
	 if (GetState() == MINIMARIO_STATE_WALKING_LEFT)
		if (this->x <= tempLocationX)
		{
			this->x = tempLocationX;
			SetState(MINIMARIO_STATE_IDLE);
		}
	 if (GetState() == MINIMARIO_STATE_WALKING_DOWN)
		if (this->y >= tempLocationY)
		{
			this->y = tempLocationY;
			SetState(MINIMARIO_STATE_IDLE);
		}
	if (GetState() == MINIMARIO_STATE_WALKING_UP)
		if (this->y <= tempLocationY)
		{
			this->y = tempLocationY;
			SetState(MINIMARIO_STATE_IDLE);
		}


	// No collision occured, proceed normally
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

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?

		if (rdx != 0 && rdx != dx)
			x += nx * abs(rdx);
		if (rdy != 0 && rdy != dy)
			y += ny * abs(rdy);

		// block every object first!
		//x += min_tx * dx + nx * 0.4f;
		//y += min_ty * dy + ny * 0.4f;

		float temp = vy;
		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		//
		// Collision logic with other objects
		//
		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			SetState(MINIMARIO_STATE_IDLE);
		}
	}

	DebugOut(L"x = %f\n", this->x);
	DebugOut(L"y = %f\n", this->y);

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

}

void CMiniMario::Render()
{
	int ani = MINIMARIO_ANI_SMALL;
	if (level == MINIMARIO_LEVEL_BIG)
		ani = MINIMARIO_ANI_BIG;
	else if (level == MINIMARIO_LEVEL_FIRE)
		ani = MINIMARIO_ANI_FIRE;
	else if (level == MINIMARIO_LEVEL_RACOON)
		ani = MINIMARIO_ANI_RACOON;
	int alpha = 255;
	animation_set->at(ani)->Render(round(x), round(y), alpha);
	//RenderBoundingBox();
}

void CMiniMario::SetState(int state)
{
	CGameObject::SetState(state);

	switch (state)
	{
	case MINIMARIO_STATE_WALKING_RIGHT:
	{
		tempLocationX = this->x + 30.0f;
		tempLocationY = this->y;
		vx = MINIMARIO_WALKING_SPEED;
		vy = 0; 
	}
		break;
	case MINIMARIO_STATE_WALKING_LEFT:
	{
		tempLocationX = this->x - 30.0f;
		tempLocationY = this->y;
		vx = -MINIMARIO_WALKING_SPEED;
		vy = 0; 
	}
		break;
	case MINIMARIO_STATE_WALKING_UP:
	{
		tempLocationX = this->x;
		tempLocationY = this->y - 30.0f;
		vy = -MINIMARIO_WALKING_SPEED;
		vx = 0;
	}
		break;
	case MINIMARIO_STATE_WALKING_DOWN:
	{
		tempLocationX = this->x;
		tempLocationY = this->y + 30.0f;
		vy = MINIMARIO_WALKING_SPEED;
		vx = 0;
	}
		break;
	case MINIMARIO_STATE_IDLE:
		vx = 0;
		vy = 0;
		break;
	}
}

void CMiniMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x + 2.0f;
	top = y + 2.0f;
	right = x + MINIMARIO_SMALL_BBOX_WIDTH ;
	bottom = y + MINIMARIO_SMALL_BBOX_HEIGHT;
}

/*
	Reset Mario status to the beginning state of a scene
*/

int CMiniMario::getLevel()
{
	return this->level;
}

