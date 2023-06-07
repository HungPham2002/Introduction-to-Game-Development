#include "FlyGoomba.h"
#include "Goomba.h"
#include "Brick.h"
#include "Utils.h"
CFlyGoomba::CFlyGoomba()
{
	SetState(FLYGOOMBA_STATE_FLYING);
}

void CFlyGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x;
	top = y;
	right = x + FLYGOOMBA_BBOX_WIDTH;
	if (state == FLYGOOMBA_STATE_WALKING)
	{
		bottom = y + FLYGOOMBA_BBOX1_HEIGHT;
		right = x + FLYGOOMBA_BBOX1_WIDTH;
	}
	else if (state == FLYGOOMBA_STATE_DIE)
		bottom = y + FLYGOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + FLYGOOMBA_BBOX_HEIGHT;
}

void CFlyGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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
			SetState(FLYGOOMBA_STATE_FLYING);
			SetPosition(this->GetInitialX(), this->GetInitialY());
		}
	}

	if (isFinish && y >= LINE_OF_DEATH)
		this->y = LINE_OF_DEATH;

	if (isFinish && dying)	// if dying and die animation finish then return
		return;

	CGameObject::Update(dt);


	if (state != FLYGOOMBA_STATE_DIE)	// if goomba not die or die deflect then have vy
		vy += FLYGOOMBA_GRAVITY * dt;

	if (state == FLYGOOMBA_STATE_DIE)
		if (GetTickCount64() - die_start > FLYGOOMBA_DYING_TIME)
		{
			die_start = 0;
			dying = 1;
		}


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != FLYGOOMBA_STATE_DIE && state != FLYGOOMBA_STATE_DIE_DEFLECT)
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

		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;

		if (ny != 0)
		{
			if (state == FLYGOOMBA_STATE_FLYING)
				vy = -FLYGOOMBA_DEFLECT_SPEED;
			else 
				vy = 0;
		}
		float temp = vy;

		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CUpsideBrick*>(e->obj))	// if e->obj is UpsideBrick 
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);
				if (state == FLYGOOMBA_STATE_FLYING)
				{
					if (e->ny > 0 || this->y + FLYGOOMBA_BBOX_HEIGHT > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						x += dx;
						y += dy;
					}
				}
				else
				{
					if (e->ny > 0 || this->y + FLYGOOMBA_BBOX1_HEIGHT > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						x += dx;
						y += dy;
					}
				}
			}
			else
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
				if (abs(ny) > 0.0001f)
					vy = -vy;
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}
}

void CFlyGoomba::Render()
{
	if (isFinish && dying)
		return;
	int ani = FLYGOOMBA_ANI_WALKING;
	if (state == FLYGOOMBA_STATE_FLYING)
		ani = FLYGOOMBA_ANI_FLYING;
	else if (state == FLYGOOMBA_STATE_DIE)
		ani = FLYGOOMBA_ANI_DIE;
	else if (state == FLYGOOMBA_STATE_DIE_DEFLECT)
		ani = FLYGOOMBA_ANI_DIE_DEFLECT;

	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CFlyGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FLYGOOMBA_STATE_DIE:
		y += FLYGOOMBA_BBOX1_HEIGHT - FLYGOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		isFinish = 1;
		StartDying();
		break;
	case FLYGOOMBA_STATE_DIE_DEFLECT:
		isFinish = 1;
		vy = -FLYGOOMBA_DIE_DEFLECT_SPEED;
		break;
	case FLYGOOMBA_STATE_FLYING:
		vx = -FLYGOOMBA_WALKING_SPEED;
		break;
	case FLYGOOMBA_STATE_WALKING:
		//y += FLYGOOMBA_BBOX_HEIGHT - FLYGOOMBA_BBOX1_HEIGHT + 1;
		vy = 0;
		break;
	}
}
