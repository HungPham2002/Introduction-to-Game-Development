#pragma once
#include "GameObject.h"

#define MUSHROOM_BBOX_WIDTH  16
#define MUSHROOM_BBOX_HEIGHT 16

#define MUSHROOM_RISING_TIME 500
#define MUSHROOM_GRAVITY	0.001f

#define MUSHROOM_STATE_MOVING 100
#define MUSHROOM_STATE_FLY		200

#define MUSHROOM_ANI_RED 0
#define MUSHROOM_ANI_GREEN 1

#define MUSHROOM_MOVING_SPEED 0.05f 
#define MUSHROOM_FLY_SPEED 0.2f 

class CMushRoom : public CGameObject
{
	DWORD rise_start;
	int rising = 0;

public:
	int color = 0;	// 0 red, 1 green
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CMushRoom( int nx, int color);
	void StartRising() { rising = 1; rise_start = DWORD(GetTickCount64()); }
	virtual void SetState(int state);
};