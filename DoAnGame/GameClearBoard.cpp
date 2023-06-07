#include "GameClearBoard.h"

CGameClearBoard::CGameClearBoard()
{
	SetState(BOARD_STATE_EMPTY);
}

void CGameClearBoard::Render()
{
	int ani = BOARD_ANI_EMPTY;
	if (state == BOARD_STATE_FLOWER)
		ani = BOARD_ANI_FLOWER;
	else if (state == BOARD_STATE_MUSHROOM)
		ani = BOARD_ANI_MUSHROOM;
	else if (state == BOARD_STATE_STAR)
		ani = BOARD_ANI_STAR;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CGameClearBoard::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}

void CGameClearBoard::SetState(int state)
{
	CGameObject::SetState(state);
}
