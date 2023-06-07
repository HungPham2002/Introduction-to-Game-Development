#pragma once
#include "GameObject.h"
#include "Environment.h"

#define ITEM_BBOX_WIDTH  16
#define ITEM_BBOX_HEIGHT 16


#define ITEM_ANI_ITEM 0
#define ITEM_ANI_STAR 1
#define ITEM_ANI_MUSHROOM 2
#define ITEM_ANI_FLOWER 3

#define GAMECLEAR_ANI_SET_ID 116

class CEndPointItem : public CGameObject
{
public:
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	int sparkling = 0;
	CEndPointItem();
	void randomitem();
	int GetItem() { return sparkling; }
};