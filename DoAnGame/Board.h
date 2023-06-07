#pragma once
#include "GameObject.h"
#include "Game.h"

#define BOARD_HEIGHT 52


class CBoard : public CGameObject
{
public:
	virtual void Render();
	virtual void Update(float camx, float camy);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};