#include "BrickAttack.h"

CBrickAttack::CBrickAttack( float maxX)
{
	vx = 0;
	vy = -BRICK_ATTACK_SPEED;
	this->maxX = maxX;
}

void CBrickAttack::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)	//turn of boundingbox
		return;
	l = x;
	t = y;
	r = x + TAIL_BBOX_WIDTH;
	b = y + TAIL_BBOX_HEIGHT;
}

void CBrickAttack::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();

	if (this->y < maxX)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);

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

		x += dx;
		y += dy;

		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CGoomba*>(e->obj))						// object is goomba
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
				goomba->vx = 0.05f * this->nx;
				game->AddScore(100);
			}
			else if (dynamic_cast<CFlyGoomba*>(e->obj))				// object is Flygoomba
			{
				CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
				goomba->SetState(FLYGOOMBA_STATE_DIE_DEFLECT);
				goomba->vx = 0.05f * this->nx;
				game->AddScore(100);
			}
			else if (dynamic_cast<CKoopas*>(e->obj))				// object is koopas
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				koopas->SetState(KOOPAS_STATE_DIE_DEFLECT);
				game->AddScore(100);
			}
			else if (dynamic_cast<CMushRoom*>(e->obj))				// object is koopas
			{
				CMushRoom* mushroom = dynamic_cast<CMushRoom*>(e->obj);
				if (mushroom->GetState() == MUSHROOM_STATE_MOVING)
					mushroom->SetState(MUSHROOM_STATE_FLY);
			}
			else if (dynamic_cast<CFlyKoopas*>(e->obj))				// object is Flykoopas
			{
				CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
				koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT);
				game->AddScore(100);
			}
			else if (dynamic_cast<CBoomerangBros*>(e->obj))				// object is Boomerang bros
			{
				isFinish = 1;
				CBoomerangBros* bros = dynamic_cast<CBoomerangBros*>(e->obj);
				bros->SetState(BROS_STATE_DIE);
				game->AddScore(1000);
			}
		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CBrickAttack::Render()
{
	if (isFinish)
		return;
	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}