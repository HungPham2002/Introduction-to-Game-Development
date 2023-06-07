#include "SpeedBar.h"

void CSpeedBar::Render()
{
	int ani = SPEEDBAR_ANI_0;
	switch (progress)
	{
	case 1:
		ani = SPEEDBAR_ANI_1;
		break;
	case 2:
		ani = SPEEDBAR_ANI_2;
		break;
	case 3:
		ani = SPEEDBAR_ANI_3;
		break;
	case 4:
		ani = SPEEDBAR_ANI_4;
		break;
	case 5:
		ani = SPEEDBAR_ANI_5;
		break;
	case 6:
		ani = SPEEDBAR_ANI_6;
		break;
	case 7:
		ani = SPEEDBAR_ANI_FULL;
		break;
	default:
		break;
	}
	animation_set->at(ani)->Render(round(x), round(y));
}

void CSpeedBar::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}

void CSpeedBar::Update(float x, float y, int progress)
{
	this->progress = progress;
	this->x = x;
	this->y = y;
}