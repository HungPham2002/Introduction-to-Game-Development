#include "Number.h"

void CNumber::Render()
{
	int ani = NUMBER_ANI_0;
	switch (number)
	{
	case 1:
		ani = NUMBER_ANI_1;
		break;
	case 2:
		ani = NUMBER_ANI_2;
		break;
	case 3:
		ani = NUMBER_ANI_3;
		break;
	case 4:
		ani = NUMBER_ANI_4;
		break;
	case 5:
		ani = NUMBER_ANI_5;
		break;
	case 6:
		ani = NUMBER_ANI_6;
		break;
	case 7:
		ani = NUMBER_ANI_7;
		break;
	case 8:
		ani = NUMBER_ANI_8;
		break;
	case 9:
		ani = NUMBER_ANI_9;
		break;
	default:
		break;
	}
	animation_set->at(ani)->Render(round(x), round(y));
}

void CNumber::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}

void CNumber::Update(float x, float y, int number)
{
	this->number = number;
	this->x = x;
	this->y = y;
}