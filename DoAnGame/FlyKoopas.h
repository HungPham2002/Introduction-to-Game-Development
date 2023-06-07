#pragma once

#include "GameObject.h"
#include "Goomba.h"
#include "QBrick.h"
#include "FlyGoomba.h"
#include "Koopas.h"

#define FLYKOOPAS_TYPE_ON_AIR 1
#define FLYKOOPAS_TYPE_ON_GROUND 0

#define FLYKOOPAS_WALKING_SPEED 0.035f;
#define FLYKOOPAS_SPIN_SPEED 0.22f
#define FLYKOOPAS_GRAVITY		0.001f
#define FLYKOOPAS_DIE_DEFLECT_SPEED 0.25f
#define FLYKOOPAS_DEFLECT_SPEED 0.25f

#define FLYKOOPAS_ON_AIR_VY 0.025f

#define FLYKOOPAS_BBOX_WIDTH 16
#define FLYKOOPAS_BBOX_HEIGHT 26
#define FLYKOOPAS_BBOX_HEIGHT_DIE 15

#define FLYKOOPAS_STATE_WALKING 100
#define FLYKOOPAS_STATE_DIE 200
#define FLYKOOPAS_STATE_SPIN 300
#define FLYKOOPAS_STATE_DIE_DEFLECT 400
#define FLYKOOPAS_STATE_DIE_DEFLECT_OUT 500
#define FLYKOOPAS_STATE_FLYING 600

#define FLYKOOPAS_ANI_WALKING_LEFT 0
#define FLYKOOPAS_ANI_WALKING_RIGHT 1
#define FLYKOOPAS_ANI_DIE 2
#define FLYKOOPAS_ANI_SPIN_RIGHT 3
#define FLYKOOPAS_ANI_SPIN_LEFT 4
#define FLYKOOPAS_ANI_SPIN_RIGHT_DEFLECT	6
#define FLYKOOPAS_ANI_SPIN_LEFT_DEFLECT	5
#define FLYKOOPAS_ANI_DIE_DEFLECT 7
#define FLYKOOPAS_ANI_FLY_RIGHT 8
#define FLYKOOPAS_ANI_FLY_LEFT 9
#define FLYKOOPAS_ANI_RESPAWN_DIE 10
#define FLYKOOPAS_ANI_RESPAWN_DIE_DEFLECT 11

class CFlyKoopas : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	int type;
	float yMin;
	float yMax;
	int aboutToRespawn = 0;
	DWORD respawn_start = NULL;
public:
	int isHolded = 0;
	CFlyKoopas(float yMin, float yMax, int type);
	virtual void SetState(int state);
	void StartRespawn() { respawn_start = DWORD(GetTickCount64()); }
};