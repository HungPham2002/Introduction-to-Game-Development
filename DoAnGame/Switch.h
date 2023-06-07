
#pragma once
#include "GameObject.h"

#define SWITCH_BBOX_WIDTH  32
#define SWITCH_BBOX_HEIGHT 1

class CSwitch : public CGameObject
{
public:
	float toX;
	float toY;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CSwitch(float x, float y);
};