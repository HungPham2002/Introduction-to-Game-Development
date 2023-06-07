#pragma once
#include "GameObject.h"

#define BOARD_ANI_EMPTY 0
#define BOARD_ANI_STAR 1
#define BOARD_ANI_MUSHROOM 2
#define BOARD_ANI_FLOWER 3

#define BOARD_STATE_EMPTY 0
#define BOARD_STATE_STAR 100
#define BOARD_STATE_MUSHROOM 200
#define BOARD_STATE_FLOWER 300

class CGameClearBoard : public CGameObject
{
public:
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	virtual void SetState(int state);
	CGameClearBoard();
};