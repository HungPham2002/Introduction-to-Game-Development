#include "FireBall.h"

CFireBall::CFireBall(int nx)
{
	//SetState(FIREBALL_STATE_SPIN);
	vx = FIREBALL_SPIN_SPEED * nx;
	this->nx = nx;
}

void CFireBall::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)	//turn of boundingbox
		return;
	l = x;
	t = y;
	r = x + FIREBALL_BBOX_WIDTH;
	b = y + FIREBALL_BBOX_HEIGHT;
}

void CFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);

	if (x < camx || x > camx + scrw)						// delete fireball if out map
		isFinish = 1;
	if (y < camy || y > camy + scrh)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);
	vy += FIREBALL_GRAVITY * dt;

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
		y += min_ty * dy + ny * 0.4f;

		if (ny != 0)			// reflect 
			vy = -FIREBALL_DEFLECT_SPEED;

		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->nx != 0)
			{
				isFinish = 1;										//delete fireball when collide with wall
			}
			if (dynamic_cast<CGoomba*>(e->obj))						// object is goomba
			{
				isFinish = 1;
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
				goomba->vx = 0.05f * this->nx;
				game->AddScore(100);
			}
			else if (dynamic_cast<CPlant*>(e->obj))					// obj is plant
			{
				isFinish = 1;
				CPlant* plant = dynamic_cast<CPlant*>(e->obj);
				plant->SetState(PLANT_STATE_DIE);
				game->AddScore(100);
			}
			else if (dynamic_cast<CPiranhaPlant*>(e->obj))			 // obj is PiranhaPlant
			{
				isFinish = 1;
				CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
				plant->SetState(PIRANHAPLANT_STATE_DIE);
				game->AddScore(100);

			}
			else if (dynamic_cast<CFlyGoomba*>(e->obj))				// object is Flygoomba
			{	
				isFinish = 1;
				CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
				goomba->SetState(FLYGOOMBA_STATE_DIE_DEFLECT);
				goomba->vx = 0.05f * this->nx;
				game->AddScore(100);
			}
			else if (dynamic_cast<CKoopas*>(e->obj))				// object is koopas
			{
				isFinish = 1;
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				koopas->SetState(KOOPAS_STATE_DIE_DEFLECT_OUT);
				koopas->vx = 0.05f * this->nx;
				game->AddScore(100);
			}
			else if (dynamic_cast<CFlyKoopas*>(e->obj))				// object is Flykoopas
			{
				isFinish = 1;
				CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
				koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
				koopas->vx = 0.05f * this->nx;
				game->AddScore(100);
			}
			else if (dynamic_cast<CBoomerangBros*>(e->obj))				// object is Boomerang bros
			{
				isFinish = 1;
				CBoomerangBros* bros = dynamic_cast<CBoomerangBros*>(e->obj);
				bros->SetState(BROS_STATE_DIE);
				game->AddScore(1000);
			}
			else if (dynamic_cast<CBrick*>(e->obj))				// object is Brick
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				if (e->nx != 0 && this->y >= brick->y)
					isFinish = 1;
				else
					isFinish = 0;
			}

		}
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void CFireBall::Render()
{
	if (isFinish)
		return;
	int ani = FIREBALL_SPIN_RIGHT;
	if (vx > 0) ani = FIREBALL_SPIN_RIGHT;
	else if (vx <= 0) ani = FIREBALL_SPIN_LEFT;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}


void CFireBall::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FIREBALL_STATE_SPIN:
		vx = FIREBALL_SPIN_SPEED;
		break;
	}
}