#include "Object.h"

void CObject::Render()
{
	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CObject::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + OBJECT_BBOX_WIDTH;
	b = y + OBJECT_BBOX_HEIGHT;
}
