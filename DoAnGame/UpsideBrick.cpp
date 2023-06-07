#pragma once
#include "UpsideBrick.h"

void CUpsideBrick::Render()
{
	animation_set->at(0)->Render(x, y);
}

void CUpsideBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + UPSIDEBRICK_BBOX_WIDTH;
	b = y + UPSIDEBRICK_BBOX_HEIGHT;
}
