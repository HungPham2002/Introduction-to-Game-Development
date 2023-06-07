#include "Item.h"

void CItem::Render()
{
	int ani = ITEMS_ANI_NONE;
	if (state == ITEMS_STATE_STAR)
		ani = ITEMS_ANI_STAR;
	else if (state == ITEMS_STATE_MUSHROOM)
		ani = ITEMS_ANI_MUSHROOM;
	else if (state == ITEMS_STATE_FLOWER)
		ani = ITEMS_ANI_FLOWER;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CItem::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}
void CItem::SetState(int state)
{
	CGameObject::SetState(state);
}
