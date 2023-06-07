
#pragma once
#include "GameObject.h"
#include "Game.h"



#define MINIMARIO_WALKING_SPEED			0.13f 

#define MINIMARIO_STATE_IDLE			0
#define MINIMARIO_STATE_WALKING_RIGHT	100
#define MINIMARIO_STATE_WALKING_LEFT	200
#define MINIMARIO_STATE_WALKING_UP		300
#define MINIMARIO_STATE_WALKING_DOWN	400

#define MINIMARIO_ANI_SMALL				0
#define MINIMARIO_ANI_BIG				1
#define MINIMARIO_ANI_FIRE				2
#define MINIMARIO_ANI_RACOON			3


#define	MINIMARIO_LEVEL_SMALL			1
#define	MINIMARIO_LEVEL_BIG				2
#define MINIMARIO_LEVEL_RACOON			3
#define MINIMARIO_LEVEL_FIRE			4

#define MINIMARIO_SMALL_BBOX_WIDTH  12
#define MINIMARIO_SMALL_BBOX_HEIGHT 11


class CMiniMario : public CGameObject
{

	float start_x;			// initial position of Mario at scene
	float start_y;
	float tempLocationX =  0.0f;
	float tempLocationY = 0.0f;
public:


	float toX = 0;
	float toY = 0;
	CMiniMario(float x = 0.0f, float y = 0.0f);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();

	void SetState(int state);
	void SetLevel(int l) { level = l; }

	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	int getLevel();

};