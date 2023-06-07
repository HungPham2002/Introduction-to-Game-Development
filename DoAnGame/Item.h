#pragma once
#include "GameObject.h"

#define ITEMS_ANI_NONE 0
#define ITEMS_ANI_STAR 1
#define ITEMS_ANI_MUSHROOM 2
#define ITEMS_ANI_FLOWER 3

#define ITEMS_STATE_NONE 0
#define ITEMS_STATE_STAR 1
#define ITEMS_STATE_MUSHROOM 2
#define ITEMS_STATE_FLOWER 3


class CItem : public CGameObject
{
public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	void SetState(int state);
};