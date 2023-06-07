#pragma once

#include "GameObject.h"
#include "Goomba.h"
#include "QBrick.h"
#include "FlyGoomba.h"
#include "FlyKoopas.h"
#include "BrokenBrick.h"
#include "Plant.h"
#include "PiranhaPlant.h"

#define KOOPAS_WALKING_SPEED 0.035f
#define KOOPAS_SPIN_SPEED 0.22f
#define KOOPAS_GRAVITY		0.001f
#define KOOPAS_DIE_DEFLECT_SPEED 0.3f

#define KOOPAS_BBOX_WIDTH 16
#define KOOPAS_BBOX_HEIGHT 26
#define KOOPAS_BBOX_HEIGHT_DIE 15

#define KOOPAS_STATE_WALKING 100
#define KOOPAS_STATE_DIE 200
#define KOOPAS_STATE_SPIN 300
#define KOOPAS_STATE_DIE_DEFLECT 400
#define KOOPAS_STATE_DIE_DEFLECT_OUT 500

#define KOOPAS_ANI_WALKING_LEFT 0
#define KOOPAS_ANI_WALKING_RIGHT 1
#define KOOPAS_ANI_DIE 2
#define KOOPAS_ANI_SPIN_RIGHT 3
#define KOOPAS_ANI_SPIN_LEFT 4
#define KOOPAS_ANI_SPIN_RIGHT_DEFLECT	6
#define KOOPAS_ANI_SPIN_LEFT_DEFLECT	5
#define KOOPAS_ANI_DIE_DEFLECT 7
#define KOOPAS_ANI_RESPAWN_DIE 8
#define KOOPAS_ANI_RESPAWN_DIE_DEFLECT 9

#define KOOPAS_RED_ANI_WALKING_LEFT 10
#define KOOPAS_RED_ANI_WALKING_RIGHT 11
#define KOOPAS_RED_ANI_DIE 12
#define KOOPAS_RED_ANI_SPIN_RIGHT 13
#define KOOPAS_RED_ANI_SPIN_LEFT 14
#define KOOPAS_RED_ANI_SPIN_RIGHT_DEFLECT	15
#define KOOPAS_RED_ANI_SPIN_LEFT_DEFLECT	16
#define KOOPAS_RED_ANI_DIE_DEFLECT 17
#define KOOPAS_RED_ANI_RESPAWN_DIE 18
#define KOOPAS_RED_ANI_RESPAWN_DIE_DEFLECT 19

#define RESPAWN_TIME 7000

class CKoopas : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	DWORD respawn_start = NULL;
	int type;
public:
	int isHolded = 0;
	float xMax = 0;
	float xMin = 0;
	int aboutToRespawn = 0;
	CKoopas( int type);
	virtual void SetState(int state);
	void StartRespawn() { respawn_start = DWORD(GetTickCount64()); }
};