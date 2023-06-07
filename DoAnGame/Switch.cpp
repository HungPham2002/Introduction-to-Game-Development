#include "Switch.h"

void CSwitch::Render()
{
	animation_set->at(0)->Render(x, y);
	//RenderBoundingBox();
}

void CSwitch::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + SWITCH_BBOX_WIDTH;
	b = y + SWITCH_BBOX_HEIGHT;
}

CSwitch::CSwitch(float x, float y)
{
	this->toX = x;
	this->toY = y;
}