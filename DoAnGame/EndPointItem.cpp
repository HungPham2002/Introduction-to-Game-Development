#include "EndPointItem.h"
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include "Utils.h" 

CEndPointItem::CEndPointItem()
{

}



void CEndPointItem::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	if (sparkling == 0)
	{
		l = x;
		t = y;
		r = x + ITEM_BBOX_WIDTH;
		b = y + ITEM_BBOX_HEIGHT;
	}
}

void CEndPointItem::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish)
		return;
	CGameObject::Update(dt);
	if (sparkling > 0)
		this->y -= 1.5;	// Item accelerates
}

void CEndPointItem::Render()
{
	if (isFinish)		//delete
		return;
	int ani = ITEM_ANI_ITEM;
	if (sparkling == ITEM_ANI_STAR)
		ani = ITEM_ANI_STAR;
	else if (sparkling == ITEM_ANI_FLOWER)
		ani = ITEM_ANI_FLOWER;
	else if (sparkling == ITEM_ANI_MUSHROOM)
		ani = ITEM_ANI_MUSHROOM;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CEndPointItem::randomitem()
{
	srand(time(NULL));
	/* generate random item between 1 and 3: */
	sparkling = rand() % 3 + 1;
}
