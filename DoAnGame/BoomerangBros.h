#pragma once
#include "Game.h"
#include "GameObject.h"
#include "Boomerang.h"

#define BROS_WALKING_SPEED	0.035f
#define BROS_GRAVITY		0.001f

#define BROS_BOOMERANG_BBOX_WIDTH 24
#define BROS_BOOMERANG_BBOX_HEIGHT 29
#define BROS_BBOX_WIDTH		16
#define BROS_BBOX_HEIGHT		24

#define BROS_STATE_WALKING				 100
#define BROS_STATE_DIE					 200
#define BROS_STATE_WALKING_NO_BOOMERANG	 300

#define BROS_ANI_WALKING				 0
#define BROS_ANI_WALKING_NO_BOOMERANG	 1
#define BROS_ANI_DIE					 2

#define BOOMERANG_ANI_SET_ID	332

#define BROS_LOADBOOMERANG_TIME 4000


class CBoomerangBros : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();

public:
	float xMax = 0;
	float xMin = 0;
	int boomerang = 0;
	int Load = 0;
	DWORD load_start;
	CBoomerangBros(float max, float min);
	CGameObject* NewBoomerang();
	virtual void SetState(int state);

	void LoadBoomerang() { Load = 1; load_start = DWORD(GetTickCount64()); }
};