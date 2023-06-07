#include "Flower.h"
#include "Utils.h"

CFlower::CFlower(float y)
{
	this->max = y - FLOWER_BBOX_HEIGHT;
}

void CFlower::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = l + FLOWER_BBOX_WIDTH;
	b = y + FLOWER_BBOX_HEIGHT;
}

void CFlower::Render()
{
	if (isFinish)
		return;

	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CFlower::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish)
		return;
	CGameObject::Update(dt);

	if (this->y > this->max)
		y += FLOWER_RISING_SPEED;

	if (this->y < this->max)
		y = max;
}


