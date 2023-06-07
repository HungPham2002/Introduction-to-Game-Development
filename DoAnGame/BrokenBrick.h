#pragma once
#include "GameObject.h"
#include "Brick.h"
#include "Fragment.h"
#include "BrickAttack.h"

#define BROKENBRICK_ANI_BRICK	0
#define BROKENBRICK_ANI_COIN	1

#define BROKENBRICK_STATE_BRICK 0 
#define BROKENBRICK_STATE_COIN	1

class CBrokenBrick : public CGameObject
{
public:
	int trigger = 0;
	int attack = 0;
	CBrokenBrick();
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	vector<CGameObject*> Broken();
	CGameObject* Attack();
	virtual void SetState(int state);
};