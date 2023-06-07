#include "BoomerangBros.h"
#include "Utils.h"

#define OBJECT_TYPE_BOOMERANG 26

CBoomerangBros::CBoomerangBros(float max, float min)
{
	xMax = round(max);
	xMin = round(min);
	SetState(BROS_STATE_WALKING);
	LoadBoomerang();
}

void CBoomerangBros::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x + BROS_BOOMERANG_BBOX_WIDTH - BROS_BBOX_WIDTH;
	top = y + BROS_BOOMERANG_BBOX_HEIGHT - BROS_BBOX_HEIGHT;
	right = left + BROS_BBOX_WIDTH;
	bottom = top + BROS_BBOX_HEIGHT;
}

void CBoomerangBros::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{

	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrh = float(game->GetScreenHeight());
	float scrw = float(game->GetScreenWidth());
	game->GetCamPos(camx, camy);
	if (y < camy || y > camy + scrh)		// out screen height then delete
		return;
	if (x > camx + scrw || x + BROS_BBOX_WIDTH/2 < camx)		// out screen width then return
		return;

	if (state != BROS_STATE_DIE)
		if (GetTickCount64() - load_start > BROS_LOADBOOMERANG_TIME)		//load boomerang falling time
		{
			load_start = 0;
			Load = 0;
			this->boomerang = 1;
			LoadBoomerang();
			SetState(BROS_STATE_WALKING_NO_BOOMERANG);
		}

	CGameObject::Update(dt);

	//
	// TO-DO: make sure Koopas can interact with the world and to each of them too!
	// 
	vy += BROS_GRAVITY * dt;

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != BROS_STATE_DIE)
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
		//y += min_ty * dy + ny * 0.4f;

		if (ny != 0) vy = 0;

		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CBoomerang*>(e->obj)) // if e->obj is Goomba 
			{
				CBoomerang* boomerang = dynamic_cast<CBoomerang*>(e->obj);
				boomerang->isFinish = 1;
				SetState(BROS_STATE_WALKING);
			}
			else if (abs(nx) > 0.0001f)
				vx = -vx;
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];

	if (state != BROS_STATE_DIE)
	{
		if (vx < 0 && x < xMax)
		{
			x = xMax; vx = -vx;
		}
		else if (vx > 0 && x > xMin)
		{
			x = xMin; vx = -vx;
		}
	}

}



void CBoomerangBros::Render()
{
	int ani = BROS_ANI_WALKING;
	if (state == BROS_STATE_WALKING_NO_BOOMERANG)
		ani = BROS_ANI_WALKING_NO_BOOMERANG;
	else if (state == BROS_STATE_DIE && vx == 0)
		ani = BROS_ANI_DIE;
	animation_set->at(ani)->Render(round(x), round(y));
}

void CBoomerangBros::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BROS_STATE_DIE:
		vx = 0;
		vy = 0;
		break;
	case BROS_STATE_WALKING:
		vx = BROS_WALKING_SPEED;
		break;
	}

}

CGameObject* CBoomerangBros::NewBoomerang()		// create fireball function
{
	int ani_set_id = BOOMERANG_ANI_SET_ID;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;
	obj = new CBoomerang(this->y - 8.0f);
	obj->type = OBJECT_TYPE_BOOMERANG;
	obj->SetPosition(this->x, this->y - 8.0f);
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);
	return obj;
}
