#include "Plant.h"
#include "Utils.h"

CPlant::CPlant(CGameObject*player, float y)
{
	SetState(PLANT_STATE_NORMAL);
	this->player = player;
	min = y;
	max = y - PLANT_BBOX_HEIGHT;
	StartRising();
}

void CPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + PLANT_BBOX_WIDTH;
	b = y + PLANT_BBOX_HEIGHT;
}

void CPlant::Render()
{
	if (isFinish && dying)
		return;
	int ani = 0;
	if (state == PLANT_STATE_DIE)
		ani = PLANT_ANI_DIE;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	game->GetCamPos(camx, camy);
	if (x > camx + scrw)		// out screen width then return
		return;
	if (isFinish && dying)
		return;
	CGameObject::Update(dt);

	if (GetTickCount64() - rise_start > PLANT_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
		if (player->y >= this->y - 30 && player->x + 25 > this->x && player->x < this->x + 25);
		else 
			StartRising();
	}

	if (isFinish)
	{
		if (GetTickCount64() - die_start > PLANT_DYING_TIME)
		{
			die_start = 0;
			dying = 1;
		}
		rising = 0;
	}

	if (rising)
	{
		isUnderPipe = 0;
		if (GetTickCount64() - rise_start >= 0 && GetTickCount64() - rise_start <= 1500)
			y -= 0.25;
		else if (GetTickCount64() - rise_start >= 1500 && GetTickCount64() - rise_start <= 2500)
		{
			y = max;
		}
		else if (GetTickCount64() - rise_start >= 2500 && GetTickCount64() - rise_start <= 4000)
			y += 0.25;
		else {
			isUnderPipe = 1;
			y = min;
		}
	}
}

void CPlant::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case PLANT_STATE_DIE:
		isFinish = 1;
		StartDying();
		break;
	case PLANT_STATE_NORMAL:
		break;
	}
}