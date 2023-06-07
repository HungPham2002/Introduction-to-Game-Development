#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Game.h"
#include "Goomba.h"
#include "FlyGoomba.h"
#include "Koopas.h"
#include "FlyKoopas.h"
#include "Plant.h"
#include "PiranhaPlant.h"
#include "BoomerangBros.h"

#define FIREBALL_BBOX_WIDTH  8
#define FIREBALL_BBOX_HEIGHT 9

#define FIREBALL_SPIN_RIGHT		0
#define FIREBALL_SPIN_LEFT		1
#define FIREBALL_SPIN_SPEED		0.25f 
#define FIREBALL_GRAVITY		0.001f
#define FIREBALL_DEFLECT_SPEED  0.15f

#define FIREBALL_STATE_SPIN		100


class CFireBall : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	CFireBall(int nx);
	void SetState(int state);
};