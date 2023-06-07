#pragma once
#include "GameObject.h"
#include "Mario.h"

#define PLANT_BBOX_WIDTH  16
#define PLANT_BBOX_HEIGHT 24

#define PLANT_RISING_TIME 6000
#define PLANT_DYING_TIME 800

#define PLANT_STATE_NORMAL			0
#define PLANT_STATE_DIE				1

#define PLANT_ANI_NORMAL			0
#define PLANT_ANI_DIE				1

class CPlant : public CGameObject
{
	DWORD rise_start;
	int rising = 0;
	CGameObject *player = NULL;
	float min;
	float max;
public:
	int dying;
	int isUnderPipe = 0;
	DWORD die_start = NULL;				//time die animation
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void SetState(int state);
	CPlant(CGameObject* player, float y);
	void StartRising() { rising = 1; rise_start = DWORD(GetTickCount64()); }
	void StartDying() { dying = 0; die_start = DWORD(GetTickCount64()); }
};