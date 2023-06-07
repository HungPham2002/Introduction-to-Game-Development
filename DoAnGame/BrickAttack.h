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
#include "QBrick.h"
#include "BrokenBrick.h"
#include "MushRoom.h"
#include "BoomerangBros.h"

#define BRICKATTACK_BBOX_WIDTH  16
#define BRICKATTACK_BBOX_HEIGHT 2

#define BRICK_ATTACK_SPEED		0.3f 


class CBrickAttack : public CGameObject
{
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Render();
public:
	float maxX = 0.0f;
	CBrickAttack( float maxX);
};