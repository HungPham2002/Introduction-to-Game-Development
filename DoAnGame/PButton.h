#pragma once
#include "GameObject.h"

#define BUTTON_BBOX_WIDTH  16
#define BUTTON_BBOX_HEIGHT 16

#define BUTTON_BBOX_HEIGHT_STOMPED 7

#define BUTTON_STATE_BUTTON 200
#define BUTTON_STATE_STOMPED 300

#define BUTTON_ANI_BUTTON 0
#define BUTTON_ANI_STOMPED 1

class CPButton : public CGameObject
{
public:
	int trigger = 0;
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	CPButton();
	virtual void SetState(int state);
};