#pragma once
#include "GameObject.h"
#include "Game.h"
#include "Koopas.h"

#define GOOMBA_WALKING_SPEED 0.035f;
#define GOOMBA_DIE_DEFLECT_SPEED	 0.3f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 15
#define GOOMBA_BBOX_HEIGHT_DIE 9
#define GOOMBA_GRAVITY		0.001f

#define GOOMBA_STATE_WALKING		100
#define GOOMBA_STATE_DIE			200
#define GOOMBA_STATE_DIE_DEFLECT	300

#define GOOMBA_ANI_WALKING 0
#define GOOMBA_ANI_DIE 1
#define GOOMBA_ANI_DIE_DEFLECT 2

#define GOOMBA_DYING_TIME	200

class CGoomba : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	DWORD die_start = NULL;				//time die animation
public:
	int dying;
	CGoomba();
	virtual void SetState(int state);
	void StartDying() { dying = 0; die_start = DWORD(GetTickCount64()); }	
};