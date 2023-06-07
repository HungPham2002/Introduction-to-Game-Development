#pragma once
#include "GameObject.h"
#include "Game.h"

#define NUMBER_HEIGHT 7
#define NUMBER_WIDTH 8

#define NUMBER_ANI_0	0
#define NUMBER_ANI_1	1
#define NUMBER_ANI_2	2
#define NUMBER_ANI_3	3
#define NUMBER_ANI_4	4
#define NUMBER_ANI_5	5
#define NUMBER_ANI_6	6
#define NUMBER_ANI_7	7
#define NUMBER_ANI_8	8
#define NUMBER_ANI_9	9



class CNumber : public CGameObject
{
private:
	int number = 0;
public:
	virtual void Render();
	virtual void Update(float x, float y, int number);
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);

	int getNum() { return this->number; }
	void setNum(int num) { this->number = num; }
};