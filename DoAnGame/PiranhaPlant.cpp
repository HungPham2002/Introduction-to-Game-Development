#include "PiranhaPlant.h"
#include "Utils.h"

#define OBJECT_TYPE_PLANTFIREBALL	17

CPiranhaPlant::CPiranhaPlant(CGameObject* player, float y)
{
	SetState(PIRANHAPLANT_STATE_NORMAL);
	this->player = player;
	min = y;
	max = y - PIRANHAPLANT_BBOX_HEIGHT;
	StartRising();
}

void CPiranhaPlant::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + PIRANHAPLANT_BBOX_WIDTH;
	b = y + PIRANHAPLANT_BBOX_HEIGHT;
}

void CPiranhaPlant::Render()
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
	int ani = PIRANHAPLANT_ANI_BOTLEFT;
	if (state == PIRANHAPLANT_STATE_DIE)
		ani = PIRANHAPLANT_ANI_DIE;
	else if (player->x < this->x && player->y >this->y)
	{
		if (climax)
			ani = PIRANHAPLANT_ANI_BOTLEFT_STILL;
	}
	else if (player->x < this->x && player->y < this->y)
	{
		if (climax)
			ani = PIRANHAPLANT_ANI_TOPLEFT_STILL;
		else
			ani = PIRANHAPLANT_ANI_TOPLEFT;
	}
	else if (player->x > this->x && player->y > this->y)
	{
		if (climax)
			ani = PIRANHAPLANT_ANI_BOTRIGHT_STILL;
		else
			ani = PIRANHAPLANT_ANI_BOTRIGHT;
	}
	else if (player->x > this->x && player->y < this->y)
	{
		if (climax)
			ani = PIRANHAPLANT_ANI_TOPRIGHT_STILL;
		else
			ani = PIRANHAPLANT_ANI_TOPRIGHT;
	}
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CPiranhaPlant::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish && dying)
		return;
	CGameObject::Update(dt);


	if (GetTickCount64() - rise_start > PIRANHAPLANT_RISING_TIME)
	{
		rise_start = 0;
		rising = 0;
		if (player->y >= this->y - 30 && player->x + 25 > this->x && player->x < this->x + 25);
		else
			StartRising();
	}

	if (isFinish)
	{
		if (GetTickCount64() - die_start > PIRANHAPLANT_DYING_TIME)
		{
			die_start = 0;
			dying = 1;
		}
		rising = 0;
	}

	if (player->x < this->x)
		nx = -1;
	else
		nx = 1;

	if (player->y < this->y)
		ny = -1;
	else
		ny = 1;


	if (rising)
	{
		isUnderPipe = 0;
		if (GetTickCount64() - rise_start >= 0 && GetTickCount64() - rise_start <= 2000)
			y -= 0.25;
		else if (GetTickCount64() - rise_start >= 2000 && GetTickCount64() - rise_start <= 3000)
		{
			y = max;
			if (GetTickCount64() - rise_start == 2500)
				this->fireball++;
		}
		else if (GetTickCount64() - rise_start >= 3000 && GetTickCount64() - rise_start <= 5000)
			y += 0.25;
		else {
			isUnderPipe = 1;
			y = min;
		}
	}

	if (y == max)
		climax = 1;
	else
		climax = 0;

	//DebugOut(L"\tclimax = %f\n",climax);
}

void CPiranhaPlant::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case PIRANHAPLANT_STATE_DIE:
		isFinish = 1;
		StartDying();
		break;
	case PIRANHAPLANT_STATE_NORMAL:
		break;
	}
}

CGameObject* CPiranhaPlant::NewFireBall()		// create fireball function
{
	int ani_set_id = 13;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;
	obj = new CPlantFireBall(this->nx, this->ny);
	obj->type = OBJECT_TYPE_PLANTFIREBALL;
	obj->SetPosition(this->x + PIRANHAPLANT_BBOX_WIDTH / 3, this->y);
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);
	return obj;
}

