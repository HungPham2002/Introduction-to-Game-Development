#include "PlantFireBall.h"

CPlantFireBall::CPlantFireBall(int nx, int ny)
{
	//SetState(FIREBALL_STATE_SPIN);
	vx = PLANTFIREBALL_SPIN_SPEED_VX * nx;
	vy = PLANTFIREBALL_SPIN_SPEED_VY * ny;
	this->nx = nx;
}

void CPlantFireBall::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)	//turn of boundingbox
		return;
	l = x;
	t = y;
	r = x + PLANTFIREBALL_BBOX_WIDTH;
	b = y + PLANTFIREBALL_BBOX_HEIGHT;
}

void CPlantFireBall::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	float scrw = float(game->GetScreenWidth());
	float scrh = float(game->GetScreenHeight());
	game->GetCamPos(camx, camy);

	if (x < camx || x > camx + scrw)						// delete fireball if out map
		isFinish = 1;
	if (y < camy || y > camy + scrh - BOARD_HEIGHT)
		isFinish = 1;

	if (isFinish)
		return;

	CGameObject::Update(dt);

	x += dx;
	y += dy;
}

void CPlantFireBall::Render()
{
	if (isFinish)
		return;
	int ani = PLANTFIREBALL_SPIN_RIGHT;
	if (vx <= 0) ani = PLANTFIREBALL_SPIN_LEFT;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}


void CPlantFireBall::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case PLANTFIREBALL_STATE_SPIN:
		break;
	}
}