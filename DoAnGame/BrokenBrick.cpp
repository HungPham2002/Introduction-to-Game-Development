#include "BrokenBrick.h"

#define OBJECT_TYPE_BRICKATTACK		22

CBrokenBrick::CBrokenBrick()
{
	SetState(BROKENBRICK_STATE_BRICK);
}

void CBrokenBrick::Render()
{
	if (isFinish)
		return;
	int ani = BROKENBRICK_ANI_BRICK;
	if (state == BROKENBRICK_STATE_COIN)
		ani = BROKENBRICK_ANI_COIN;
	animation_set->at(ani)->Render(round(x), round(y));
	//RenderBoundingBox();
}

void CBrokenBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	if (isFinish)
		return;
	l = x;
	t = y;
	r = x + BRICK_BBOX_WIDTH;
	b = y + BRICK_BBOX_HEIGHT;
}

void CBrokenBrick::SetState(int state)
{
	CGameObject::SetState(state);
}

vector<CGameObject*>CBrokenBrick::Broken()
{
	int ani_set_id = 106;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	vector<CGameObject*> VCGameObject;
	for (int i = 0; i < 4; i++)
	{
		CGameObject* obj = NULL;
		obj = new CFragment(i);
		obj->SetPosition(this->x, this->y);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
		VCGameObject.push_back(obj);
	}
	return VCGameObject;
}

CGameObject* CBrokenBrick::Attack()		// create attack function
{
	int ani_set_id = MARIO_TAIL_ANI_SET_ID;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	CGameObject* obj = NULL;
	float maxRange = this->y - 1.0f;
	obj = new CBrickAttack(maxRange);
	obj->type = OBJECT_TYPE_BRICKATTACK;
	obj->SetPosition(this->x, this->y + BRICK_BBOX_HEIGHT);
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	obj->SetAnimationSet(ani_set);
	return obj;
}