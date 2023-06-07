#pragma once
#include "GameObject.h"
#include "Mario.h"
#include "PlantFireBall.h"

#define PIRANHAPLANT_BBOX_WIDTH  16
#define PIRANHAPLANT_BBOX_HEIGHT 32

#define PIRANHAPLANT_ANI_BOTLEFT_STILL		0
#define PIRANHAPLANT_ANI_BOTLEFT			1
#define PIRANHAPLANT_ANI_TOPLEFT_STILL		2
#define PIRANHAPLANT_ANI_TOPLEFT			3
#define PIRANHAPLANT_ANI_BOTRIGHT_STILL		4
#define PIRANHAPLANT_ANI_BOTRIGHT			5
#define PIRANHAPLANT_ANI_TOPRIGHT_STILL		6
#define PIRANHAPLANT_ANI_TOPRIGHT			7
#define PIRANHAPLANT_ANI_DIE				8

#define PIRANHAPLANT_RISING_TIME 7000
#define PIRANHAPLANT_DYING_TIME 800

#define PIRANHAPLANT_STATE_NORMAL			0
#define PIRANHAPLANT_STATE_DIE				1

class CPiranhaPlant : public CGameObject
{
	DWORD rise_start;
	int rising = 0;
	CGameObject* player = NULL;
	float min;
	float max;
public:
	int dying;
	int isUnderPipe = 0;
	int climax = 0;
	int fireball = 0;
	int ny = 1;
	DWORD die_start = NULL;				//time die animation
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void SetState(int state);
	CPiranhaPlant(CGameObject* player, float y);
	CGameObject* NewFireBall();
	void StartRising() { rising = 1; rise_start = DWORD(GetTickCount64()); }
	void StartDying() { dying = 0; die_start = DWORD(GetTickCount64()); }
};