#include "Koopas.h"
#include "Brick.h"
#include "UpsideBrick.h"
#include "Utils.h"

CKoopas::CKoopas(int type)
{
	this->type = type;
	SetState(KOOPAS_STATE_WALKING);

}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isFinish)
		return;
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;

	if (state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT)
			bottom = y + KOOPAS_BBOX_HEIGHT_DIE;
	else
		bottom = y + KOOPAS_BBOX_HEIGHT;
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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
			SetState(KOOPAS_STATE_WALKING);
			SetPosition(this->GetInitialX(), this->GetInitialY());
		}
	}

	if (isFinish && y >= LINE_OF_DEATH)
		this->y = LINE_OF_DEATH;


	CGameObject::Update(dt);

	//
	// TO-DO: make sure Koopas can interact with the world and to each of them too!
	// 

	vy += KOOPAS_GRAVITY * dt;

	if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx == 0)
	{
		if (GetTickCount64() - respawn_start > RESPAWN_TIME)
		{
			isHolded = 0;
			SetState(KOOPAS_STATE_WALKING);
			y -= KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_DIE;
			aboutToRespawn = 0;
		}
		else if (GetTickCount64() - respawn_start > RESPAWN_TIME - 1000)
			aboutToRespawn = 1;
	}
	else if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
	{
		respawn_start = DWORD(GetTickCount64());
		aboutToRespawn = 0;
	}

	if (state == KOOPAS_STATE_WALKING)
	{
		if (abs(vx) > KOOPAS_WALKING_SPEED)
			vx = -KOOPAS_WALKING_SPEED;
	}


	if (vx != 0)
		isHolded = 0;

	if (isHolded)
		vy = 0;


	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (state != KOOPAS_STATE_DIE_DEFLECT_OUT)
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
		//if (nx != 0) vx = 0;
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
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					goomba->SetState(GOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
					game->AddScore(100);
				}
				else if (state == KOOPAS_STATE_WALKING)
				{
					vx = -vx;
					goomba->vx = -goomba->vx;
				}
			}
			else if (dynamic_cast<CFlyGoomba*>(e->obj))	// if e->obj is goomba 
			{
				CFlyGoomba* goomba = dynamic_cast<CFlyGoomba*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					goomba->SetState(FLYGOOMBA_STATE_DIE_DEFLECT);
					goomba->vx = 0.05f * this->nx;
					game->AddScore(100);
				}
				else if (state == KOOPAS_STATE_WALKING)
				{
					vx = -vx;
					goomba->vx = -goomba->vx;
				}
			}
			else if (dynamic_cast<CPlant*>(e->obj))					// obj is plant
			{
				CPlant* plant = dynamic_cast<CPlant*>(e->obj);
				plant->SetState(PLANT_STATE_DIE);
				game->AddScore(100);
			}
			else if (dynamic_cast<CPiranhaPlant*>(e->obj))			 // obj is PiranhaPlant
			{
				CPiranhaPlant* plant = dynamic_cast<CPiranhaPlant*>(e->obj);
				plant->SetState(PIRANHAPLANT_STATE_DIE);
				game->AddScore(100);
			}
			else if (dynamic_cast<CKoopas*>(e->obj))	// if e->obj is koopas
			{
				CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(KOOPAS_STATE_DIE_DEFLECT_OUT);
					//koopas->vx = 0.05f * this->nx;
					game->AddScore(100);
				}
			}
			else if (dynamic_cast<CFlyKoopas*>(e->obj))	// if e->obj is koopas
			{
				CFlyKoopas* koopas = dynamic_cast<CFlyKoopas*>(e->obj);
				if ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0)
				{
					koopas->SetState(FLYKOOPAS_STATE_DIE_DEFLECT_OUT);
					//koopas->vx = 0.05f * this->nx;
					game->AddScore(100);
				}

			}
			else if (dynamic_cast<CUpsideBrick*>(e->obj))	// if e->obj is UpsideBrick 
			{
				CUpsideBrick* Upsidebrick = dynamic_cast<CUpsideBrick*>(e->obj);
				if (state == KOOPAS_STATE_WALKING)
				{
					if (e->ny > 0 || this->y + KOOPAS_BBOX_HEIGHT > Upsidebrick->y)
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
					if (e->ny > 0 || this->y + KOOPAS_BBOX_HEIGHT_DIE > Upsidebrick->y)
					{
						//If wrong side then go through
						vy = temp;
						x -= min_tx * dx + nx * 0.4f;
						x += dx;
						y += dy;
					}
				}
			}
			else if (dynamic_cast<CQBrick*>(e->obj))		//question brick
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
				CQBrick* qbrick = dynamic_cast<CQBrick*>(e->obj);
				if (e->nx != 0 &&((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0))
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
			}
			else if (dynamic_cast<CBrokenBrick*>(e->obj))		//question brick
			{
				if (abs(nx) > 0.0001f)
					vx = -vx;
				CBrokenBrick* bbrick = dynamic_cast<CBrokenBrick*>(e->obj);
				if (e->nx != 0 && ((state == KOOPAS_STATE_DIE || state == KOOPAS_STATE_DIE_DEFLECT) && vx != 0) && this->y + KOOPAS_BBOX_HEIGHT_DIE - 1  > bbrick->y)
				{
					if (bbrick->GetState() == BROKENBRICK_STATE_BRICK)
					{
						bbrick->trigger = 1;
						bbrick->isFinish = 1;
					}
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

	//DebugOut(L"Vy = %f\n", vy);

	// if red koopas then turn when about to fall
	if (this->type == 1 && state == KOOPAS_STATE_WALKING)
	{
		if (this->vy > 0.04f)
		{
			if (vx > 0)
			{
				this->y -= 0.8f;
				this->x -= 2.0f;
				this->vx = -this->vx;
			}
			else 
			{
				this->y -= 0.8f;
				this->x += 2.0f;
				this->vx = -this->vx;
			}
		}
	}



	//DebugOut(L"x = %f\n", this->x);
	//DebugOut(L"y = %f\n", this->y);

}



void CKoopas::Render()
{
	int ani;
	if (type == 1) // RED KOOPAS
	{
		ani = KOOPAS_RED_ANI_WALKING_LEFT;
		if (aboutToRespawn)
		{
			if (state == KOOPAS_STATE_DIE)
				ani = KOOPAS_RED_ANI_RESPAWN_DIE;
			else
				ani = KOOPAS_RED_ANI_RESPAWN_DIE_DEFLECT;
		}
		else if (state == KOOPAS_STATE_DIE && vx == 0)
			ani = KOOPAS_RED_ANI_DIE;
		else if ((state == KOOPAS_STATE_DIE_DEFLECT && vx == 0) || state == KOOPAS_STATE_DIE_DEFLECT_OUT)
			ani = KOOPAS_RED_ANI_DIE_DEFLECT;
		else if (state == KOOPAS_STATE_DIE && vx > 0)
			ani = KOOPAS_RED_ANI_SPIN_RIGHT;
		else if (state == KOOPAS_STATE_DIE && vx < 0)
			ani = KOOPAS_RED_ANI_SPIN_LEFT;
		else if (state == KOOPAS_STATE_DIE_DEFLECT && vx > 0)
			ani = KOOPAS_RED_ANI_SPIN_RIGHT_DEFLECT;
		else if (state == KOOPAS_STATE_DIE_DEFLECT && vx < 0)
			ani = KOOPAS_RED_ANI_SPIN_LEFT_DEFLECT;
		else if (vx > 0) ani = KOOPAS_RED_ANI_WALKING_RIGHT;
		else if (vx <= 0) ani = KOOPAS_RED_ANI_WALKING_LEFT;
	}
	else
	{
		ani = KOOPAS_ANI_WALKING_LEFT;
		if (aboutToRespawn)
		{
			if (state == KOOPAS_STATE_DIE)
				ani = KOOPAS_ANI_RESPAWN_DIE;
			else
				ani = KOOPAS_ANI_RESPAWN_DIE_DEFLECT;
		}
		else if (state == KOOPAS_STATE_DIE && vx == 0)
			ani = KOOPAS_ANI_DIE;
		else if ((state == KOOPAS_STATE_DIE_DEFLECT && vx == 0) || state == KOOPAS_STATE_DIE_DEFLECT_OUT)
			ani = KOOPAS_ANI_DIE_DEFLECT;
		else if (state == KOOPAS_STATE_DIE && vx > 0)
			ani = KOOPAS_ANI_SPIN_RIGHT;
		else if (state == KOOPAS_STATE_DIE && vx < 0)
			ani = KOOPAS_ANI_SPIN_LEFT;
		else if (state == KOOPAS_STATE_DIE_DEFLECT && vx > 0)
			ani = KOOPAS_ANI_SPIN_RIGHT_DEFLECT;
		else if (state == KOOPAS_STATE_DIE_DEFLECT && vx < 0)
			ani = KOOPAS_ANI_SPIN_LEFT_DEFLECT;
		else if (vx > 0) ani = KOOPAS_ANI_WALKING_RIGHT;
		else if (vx <= 0) ani = KOOPAS_ANI_WALKING_LEFT;
	}

	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPAS_STATE_DIE:
		aboutToRespawn = 0;
		StartRespawn();
		y += KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT_DIE;
		vx = 0;
		vy = 0;
		break;
	case KOOPAS_STATE_DIE_DEFLECT:
		aboutToRespawn = 0;
		StartRespawn();
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		vx = 0;
		break;
	case KOOPAS_STATE_DIE_DEFLECT_OUT:
		aboutToRespawn = 0;
		vy = -KOOPAS_DIE_DEFLECT_SPEED;
		isFinish = 1;
		break;
	case KOOPAS_STATE_WALKING:
		aboutToRespawn = 0;
		vx = -KOOPAS_WALKING_SPEED;
		break;
	}

}
 