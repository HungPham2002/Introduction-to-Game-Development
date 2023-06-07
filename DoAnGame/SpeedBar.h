#pragma once
#include "GameObject.h"
#include "Game.h"


#define SPEEDBAR_ANI_0		0
#define SPEEDBAR_ANI_1		1
#define SPEEDBAR_ANI_2		2
#define SPEEDBAR_ANI_3		3
#define SPEEDBAR_ANI_4		4
#define SPEEDBAR_ANI_5		5
#define SPEEDBAR_ANI_6		6
#define SPEEDBAR_ANI_FULL	7




class CSpeedBar : public CGameObject
{
private:
	int progress = 0;
public:
	virtual void Render();
	virtual void Update(float x, float y, int progress);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

};