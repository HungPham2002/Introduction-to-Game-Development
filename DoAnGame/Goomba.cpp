#include "Goomba.h"
#include "Brick.h"
#include "Utils.h"
CGoomba::CGoomba()
{
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_BBOX_HEIGHT;
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);


	if ((y >= LINE_OF_DEATH) && !isFinish)		// out screen width then respawn
	{
		x = this->GetInitialX();
		y = LINE_OF_DEATH;
		if (this->GetInitialX() < camx || this->GetInitialX() > camx + scrw || this->GetInitialY() < camy || this->GetInitialY() > camy + scrh)
		{
			SetState(GOOMBA_STATE_WALKING);
			SetPosition(this->GetInitialX(), this->GetInitialY());
		}
	}

	if (isFinish && y >= LINE_OF_DEATH)
		this->y = LINE_OF_DEATH;


	if (isFinish && dying)	// if dying and die animation finish then return
		return;

	CGameObject::Update(dt);


	if (state != GOOMBA_STATE_DIE)	// if goomba not die or die deflect then have vy
		vy += GOOMBA_GRAVITY * dt;

	if (state == GOOMBA_STATE_DIE)
		if (GetTickCount64() - die_start > GOOMBA_DYING_TIME)
		{
			die_start = 0;
			dying = 1;
		}


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != GOOMBA_STATE_DIE && state != GOOMBA_STATE_DIE_DEFLECT)
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

		// todo: this is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);


		// block every object first!
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		float temp = vy;
		if (ny != 0) vy = 0;

		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->ny > 0)
			{
				vy = temp;
				x -= min_tx * dx + nx * 0.4f;
				y -= min_ty * dy + ny * 0.4f;
			}
			if (dynamic_cast<CGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if (abs(nx) > 0.0001f)
				{
					vx = -vx;
					goomba->vx = -goomba->vx;
				}
			}
			else
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}
}

void CGoomba::Render()
{
	if (isFinish && dying)
		return;
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE)
		ani = GOOMBA_ANI_DIE;
	else if (state == GOOMBA_STATE_DIE_DEFLECT)
		ani = GOOMBA_ANI_DIE_DEFLECT;

	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		isFinish = 1;
		StartDying();
		break;
	case GOOMBA_STATE_DIE_DEFLECT:
		isFinish = 1;
		vy = -GOOMBA_DIE_DEFLECT_SPEED;
		break;
	case GOOMBA_STATE_WALKING:
		vx = -GOOMBA_WALKING_SPEED;
		break;
	}
}
