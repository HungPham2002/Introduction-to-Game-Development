#pragma once
#include "GameObject.h"
#include "Game.h"
#include "UpsideBrick.h"

#define FLYGOOMBA_WALKING_SPEED 0.035f;
#define FLYGOOMBA_DIE_DEFLECT_SPEED	 0.25f

#define FLYGOOMBA_BBOX_WIDTH 20
#define FLYGOOMBA_BBOX_HEIGHT 23

#define FLYGOOMBA_BBOX1_WIDTH 16
#define FLYGOOMBA_BBOX1_HEIGHT 16
#define FLYGOOMBA_BBOX_HEIGHT_DIE 9
#define FLYGOOMBA_GRAVITY		0.001f
#define FLYGOOMBA_DEFLECT_SPEED 0.25f

#define	FLYGOOMBA_STATE_FLYING		100
#define FLYGOOMBA_STATE_WALKING		200
#define FLYGOOMBA_STATE_DIE			300
#define FLYGOOMBA_STATE_DIE_DEFLECT	400

#define FLYGOOMBA_ANI_FLYING 0
#define FLYGOOMBA_ANI_WALKING 1
#define FLYGOOMBA_ANI_DIE 2
#define FLYGOOMBA_ANI_DIE_DEFLECT 3

#define FLYGOOMBA_DYING_TIME	200

class CFlyGoomba : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	DWORD die_start = NULL;				//time die animation
public:
	int dying;
	CFlyGoomba();
	virtual void SetState(int state);
	void StartDying() { dying = 0; die_start = DWORD(GetTickCount64()); }
};