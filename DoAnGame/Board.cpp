#include "Board.h"

void CBoard::Render()
{
	animation_set->at(0)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CBoard::GetBoundingBox(float& l, float& t, float& r, float& b)
{

}

void CBoard::Update(float camx, float camy)
{
	CGame* game = CGame::GetInstance();
	float scrh = float(game->GetScreenHeight());
	this->x = camx;
	this->y = camy + scrh - BOARD_HEIGHT;
}