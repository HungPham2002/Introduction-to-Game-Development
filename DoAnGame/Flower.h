
#pragma once
#include "GameObject.h"

#define FLOWER_BBOX_WIDTH  16
#define FLOWER_BBOX_HEIGHT 16

#define FLOWER_RISING_SPEED  -0.28f



class CFlower : public CGameObject
{
	int rising = 0;
	float max;
public:
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CFlower(float y);
};