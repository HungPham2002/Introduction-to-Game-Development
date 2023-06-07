#include "Tag.h"

void CTag::Render()
{
	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CTag::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}
