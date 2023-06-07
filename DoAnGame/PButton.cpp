#include "PButton.h"
#include "Utils.h"

CPButton::CPButton()
{
	SetState(BUTTON_STATE_BUTTON);
}


void CPButton::Render()
{
	int ani = BUTTON_ANI_BUTTON;
	if (state == BUTTON_STATE_STOMPED)
		ani = BUTTON_ANI_STOMPED;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CPButton::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y + 3;
	r = x + BUTTON_BBOX_WIDTH;
	b = y + BUTTON_BBOX_HEIGHT;
	if (state == BUTTON_STATE_STOMPED)
		b = y + BUTTON_BBOX_HEIGHT_STOMPED;
}

void CPButton::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BUTTON_STATE_STOMPED:
		y += BUTTON_BBOX_HEIGHT - BUTTON_BBOX_HEIGHT_STOMPED;
		break;
	}

}
