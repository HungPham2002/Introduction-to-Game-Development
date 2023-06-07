#include "Coin.h"

CCoin::CCoin()
{
	sparkle_start = NULL;
}

CCoin::CCoin(int setting)
{								
	isSparkle = setting;		// create coin animation when hit the Question brick
	StartSparkling();
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish || isSparkle)
		return;
	l = x;
	t = y;
	r = x + COIN_BBOX_WIDTH;
	b = y + COIN_BBOX_HEIGHT;
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	if (isFinish)
		return;
	CGameObject::Update(dt);
	if (isSparkle)
	{
		if (GetTickCount64() - sparkle_start > COIN_SPARKLING_TIME)
		{
			sparkle_start = 0;
			sparkling = 0;
			isFinish = 1;
		}

		if (sparkling)
		{
			if (GetTickCount64() - sparkle_start >= COIN_SPARKLING_TIME / 2)
				y += COIN_VY_DOWN;
			else
				y -= COIN_VY_UP;
		}
	}
}

void CCoin::Render()
{
	if (isFinish)		//delete coin 
		return;
	int ani = COIN_ANI_COIN;
	if (isSparkle == 1)
		ani = COIN_ANI_SPARKLE;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}