#include "FlyKoopas.h"
#include "Brick.h"
#include "UpsideBrick.h"
#include "Koopas.h"
#include "Utils.h"

CFlyKoopas::CFlyKoopas(float yMin, float yMax, int type)
{
	this->type = type;
	if (this->type == FLYKOOPAS_TYPE_ON_AIR)
	{
		this->yMin = yMin;
		this->yMax = yMax;
	}
	SetState(FLYKOOPAS_STATE_FLYING);
}

void CFlyKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x;
	top = y;
	right = x + FLYKOOPAS_BBOX_WIDTH;
	if (state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT)
		bottom = y + FLYKOOPAS_BBOX_HEIGHT_DIE;
	else
		bottom = y + FLYKOOPAS_BBOX_HEIGHT;
}

void CFlyKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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
			SetState(FLYKOOPAS_STATE_FLYING);
			SetPosition(this->GetInitialX(), this->GetInitialY());
		}
	}

	if (isFinish && y >= LINE_OF_DEATH)
		this->y = LINE_OF_DEATH;

	CGameObject::Update(dt);

	//
	// TO-DO: make sure Koopas can interact with the world and to each of them too!
	// 

	if (type == FLYKOOPAS_TYPE_ON_AIR )
	{
		if (state == FLYKOOPAS_STATE_FLYING)
		{
			y += dy;
			if ( y >= yMax)
			{
				y = yMax; vy = -vy;
			}
			else if ( y <= yMin)
			{
				y = yMin; vy = -vy;
			}
			return;
		}
	}

	vy += FLYKOOPAS_GRAVITY * dt;

	if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx == 0)
	{
		if (GetTickCount64() - respawn_start > RESPAWN_TIME)
		{
			isHolded = 0;
			SetState(FLYKOOPAS_STATE_WALKING);
			y -= FLYKOOPAS_BBOX_HEIGHT - FLYKOOPAS_BBOX_HEIGHT_DIE;
			aboutToRespawn = 0;
		}
		else if (GetTickCount64() - respawn_start > RESPAWN_TIME - 1000)
			aboutToRespawn = 1;
	}
	else if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
	{
		respawn_start = DWORD(GetTickCount64());
		aboutToRespawn = 0;
	}

	if (vx != 0)
		isHolded = 0;

	if (isHolded)
		vy = 0;


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != FLYKOOPAS_STATE_DIE_DEFLECT_OUT)
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

		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block every object first!
		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.4f;
		float temp = vy;
		if (ny != 0)
		{
			if (state == FLYKOOPAS_STATE_FLYING)
				vy = -FLYKOOPAS_DEFLECT_SPEED;
			else
				vy = 0;
		}

		//if (nx != 0) vx = 0;


		for (int i = 0; i < int(coEventsResult.size()); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<CGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
					game->AddScore(100);
				}
				else
				{
					vx = -vx;
					goomba->vx = -goomba->vx;
				}
			}
			else if (dynamic_cast<CFlyGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					goomba->SetState(FLYGOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
					game->AddScore(100);
				}
				else
				{
					vx = -vx;
					goomba->vx = -goomba->vx;
				}
			}
			else if (dynamic_cast<CPlant*>(e->obj))					// obj is plant
			{
				CPlant* plant = dynamic_cast<CPlant*>(e->obj);
				plant->isFinish = 1;
				game->AddScore(100);
			}
			else if (dynamic_cast<CPiranhaPlant*>(e->obj))			 // obj is PiranhaPlant
			{
				CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
				plant->isFinish = 1;
				game->AddScore(100);
			}
			else if (dynamic_cast<CKoopas*>(e->obj))	// if e->obj is koopas
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
					game->AddScore(100);
					//koopas->vx = 0.05f * this->nx;
				}
			}
			else if (dynamic_cast<CFlyKoopas*>(e->obj))	// if e->obj is Flykoopas
			{
				CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
				if ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
					game->AddScore(100);
					//koopas->vx = 0.05f * this->nx;
				}
			}
			else if (dynamic_cast<CUpsideBrick*>(e->obj))	// if e->obj is UpsideBrick 
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);
				if (state == FLYKOOPAS_STATE_WALKING)
				{
					if (e->ny > 0 || this->y + FLYKOOPAS_BBOX_HEIGHT > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dx + ny * 0.4f;
						x += dx;
						y += dy;
					}
				}
				else
				{
					if (e->ny > 0 || this->y + FLYKOOPAS_BBOX_HEIGHT_DIE > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						y -= min_ty * dx + ny * 0.4f;
						x += dx;
						y += dy;
					}
				}
			}
			else if (dynamic_cast<CQBrick*>(e->obj))		//question brick
			{
				CQBrick* qbrick = dynamic_cast<CQBrick*>(e->obj);
				if (e->nx != 0 && ((state == FLYKOOPAS_STATE_DIE || state == FLYKOOPAS_STATE_DIE_DEFLECT) && vx != 0))
				{
					if (qbrick->GetState() != BRICK_STATE_EMP)
					{
						qbrick->trigger = 1;
						qbrick->StartRinging();
						qbrick->stack--;
						if (qbrick->stack == 0)
							qbrick->SetState(BRICK_STATE_EMP);
					}
				}
				if (abs(nx) > 0.0001f)
					vx = -vx;
				if (abs(ny) > 0.0001f)
					vy = -vy;
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



void CFlyKoopas::Render()
{
	int ani = FLYKOOPAS_ANI_WALKING_LEFT;
	if (aboutToRespawn)
	{
		if (state == FLYKOOPAS_STATE_DIE)
			ani = FLYKOOPAS_ANI_RESPAWN_DIE;
		else
			ani = FLYKOOPAS_ANI_RESPAWN_DIE_DEFLECT;
	}
	else if (state == FLYKOOPAS_STATE_DIE && vx == 0)
		ani = FLYKOOPAS_ANI_DIE;
	else if ((state == FLYKOOPAS_STATE_DIE_DEFLECT && vx == 0) || state == FLYKOOPAS_STATE_DIE_DEFLECT_OUT)
		ani = FLYKOOPAS_ANI_DIE_DEFLECT;
	else if (state == KOOPAS_STATE_DIE && vx > 0)
		ani = FLYKOOPAS_ANI_SPIN_RIGHT;
	else if (state == FLYKOOPAS_STATE_DIE && vx < 0)
		ani = FLYKOOPAS_ANI_SPIN_LEFT;
	else if (state == FLYKOOPAS_STATE_DIE_DEFLECT && vx > 0)
		ani = FLYKOOPAS_ANI_SPIN_RIGHT_DEFLECT;
	else if (state == FLYKOOPAS_STATE_DIE_DEFLECT && vx < 0)
		ani = FLYKOOPAS_ANI_SPIN_LEFT_DEFLECT;
	else if (vx > 0 && state == FLYKOOPAS_STATE_FLYING) ani = FLYKOOPAS_ANI_FLY_RIGHT;
	else if (vx <= 0 && state == FLYKOOPAS_STATE_FLYING) ani = FLYKOOPAS_ANI_FLY_LEFT;
	else if (vx > 0) ani = FLYKOOPAS_ANI_WALKING_RIGHT;
	else if (vx <= 0) ani = FLYKOOPAS_ANI_WALKING_LEFT;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CFlyKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case FLYKOOPAS_STATE_DIE:
		aboutToRespawn = 0;
		StartRespawn();
		y += FLYKOOPAS_BBOX_HEIGHT - FLYKOOPAS_BBOX_HEIGHT_DIE;
		vx = 0;
		vy = 0;
		break;
	case FLYKOOPAS_STATE_DIE_DEFLECT:
		aboutToRespawn = 0;
		StartRespawn();
		vy = -FLYKOOPAS_DIE_DEFLECT_SPEED;
		vx = 0;
		break;
	case FLYKOOPAS_STATE_DIE_DEFLECT_OUT:
		aboutToRespawn = 0;
		vy = -FLYKOOPAS_DIE_DEFLECT_SPEED;
		isFinish = 1;
		break;
	case FLYKOOPAS_STATE_WALKING:
		aboutToRespawn = 0;
		vy = 0;
		vx = -FLYKOOPAS_WALKING_SPEED;
		break;
	case FLYKOOPAS_STATE_FLYING:
		if (type == FLYKOOPAS_TYPE_ON_AIR)			
		{
			vx = 0;
			vy = FLYKOOPAS_ON_AIR_VY;
		}
		else
			vx = -FLYKOOPAS_WALKING_SPEED;
		break;
	}

}
