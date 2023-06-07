#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Board.h"

#define PLANTFIREBALL_BBOX_WIDTH  8
#define PLANTFIREBALL_BBOX_HEIGHT 9

#define PLANTFIREBALL_SPIN_RIGHT	0
#define PLANTFIREBALL_SPIN_LEFT		1
#define PLANTFIREBALL_SPIN_SPEED_VX	0.05f 
#define PLANTFIREBALL_SPIN_SPEED_VY	0.04f

#define PLANTFIREBALL_STATE_SPIN	100


class CPlantFireBall : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	CPlantFireBall(int nx, int ny);
	void SetState(int state);
};