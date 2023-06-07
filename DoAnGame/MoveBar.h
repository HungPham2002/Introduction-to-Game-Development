#pragma once
#include "GameObject.h"
#include "Game.h"

#define BAR_MOVING_SPEED 0.03f;

#define BAR_BBOX_WIDTH 48
#define BAR_BBOX_HEIGHT 16
#define BAR_GRAVITY		0.00015f

#define BAR_STATE_MOVING		100
#define BAR_STATE_FALLING		200


class CMoveBar : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
public:
	int isStomped = 0;
	CMoveBar();
	virtual void SetState(int state);
};