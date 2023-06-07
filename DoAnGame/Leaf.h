#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Board.h"

#define LEAF_BBOX_WIDTH  16
#define LEAF_BBOX_HEIGHT 14

#define LEAF_FALL_RIGHT	0
#define LEAF_FALL_LEFT	1
#define LEAF_FALL_SPEED_VX	0.04f 
#define LEAF_FALL_SPEED_VY	0.02f

#define LEAF_RISING_TIME 500


#define LEAF_STATE_FALLING	100


class CLeaf : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
	float xMax = 0;
	float xMin = 0;
	DWORD rise_start;
	int rising = 0;
public:
	CLeaf(float x);
	void StartRising() { rising = 1; rise_start = DWORD(GetTickCount64()); }
	void SetState(int state);
};