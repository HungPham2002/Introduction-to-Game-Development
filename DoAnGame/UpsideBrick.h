#pragma once
#include "GameObject.h"

#define UPSIDEBRICK_BBOX_WIDTH  16
#define UPSIDEBRICK_BBOX_HEIGHT 1

class CUpsideBrick: public CGameObject
{
public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
}; 