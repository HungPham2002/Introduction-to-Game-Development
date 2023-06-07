#include <d3dx9.h>
#include <algorithm>


#include "Utils.h"
#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"

#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_BRICK	1
#define OBJECT_TYPE_GOOMBA	2
#define OBJECT_TYPE_KOOPAS	3
#define OBJECT_TYPE_ENVIRONMENT 4
#define OBJECT_TYPE_UPSIDEBRICK 5
#define OBJECT_TYPE_COIN 6
#define OBJECT_TYPE_QBRICK 7
#define OBJECT_TYPE_FIREBALL 8
#define OBJECT_TYPE_FLYGOOMBA 9
#define OBJECT_TYPE_FLYKOOPAS 10
#define OBJECT_TYPE_PLANT 11
#define OBJECT_TYPE_PIRANHAPLANT 12
#define OBJECT_TYPE_SWITCH 13
#define OBJECT_TYPE_BROKENBRICK 14
#define OBJECT_TYPE_PBUTTON 15
#define OBJECT_TYPE_BOARD 16
#define OBJECT_TYPE_PLANTFIREBALL	17
#define OBJECT_TYPE_MUSHROOM		18
#define OBJECT_TYPE_LEAF			19
#define OBJECT_TYPE_ENDPOINTITEM	20
#define OBJECT_TYPE_GAMECLEARBOARD	21
#define OBJECT_TYPE_BRICKATTACK		22
#define OBJECT_TYPE_FLOWER				28
#define OBJECT_TYPE_PORTAL	50

CGameObject::CGameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;
	level = 0;
}

void CGameObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	this->dt = dt;
	dx = vx * dt;
	dy = vy * dt;
}

/*
	Extension of original SweptAABB to deal with two moving objects
*/
LPCOLLISIONEVENT CGameObject::SweptAABBEx(LPGAMEOBJECT coO)
{
	float sl, st, sr, sb;		// static object bbox
	float ml, mt, mr, mb;		// moving object bbox
	float t, nx, ny;

	coO->GetBoundingBox(sl, st, sr, sb);

	// deal with moving object: m speed = original m speed - collide object speed
	float svx, svy;
	coO->GetSpeed(svx, svy);

	float sdx = svx * dt;
	float sdy = svy * dt;

	// (rdx, rdy) is RELATIVE movement distance/velocity 
	float rdx = this->dx - sdx;
	float rdy = this->dy - sdy;

	GetBoundingBox(ml, mt, mr, mb);

	CGame::SweptAABB(
		ml, mt, mr, mb,
		rdx, rdy,
		sl, st, sr, sb,
		t, nx, ny
	);

	CCollisionEvent* e = new CCollisionEvent(t, nx, ny, rdx, rdy, coO);
	return e;
}

/*
	Calculate potential collisions with the list of colliable objects

	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void CGameObject::CalcPotentialCollisions(
	vector<LPGAMEOBJECT>* coObjects,
	vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));
		if (this->type == OBJECT_TYPE_MARIO)
		{
			if (e->t > 0 && e->t <= 1.0f)
			{
				if (coObjects->at(i)->type == OBJECT_TYPE_COIN)
					delete e;
				else
					coEvents.push_back(e);
			}
			else
				delete e;
		}
		else if (this->type == OBJECT_TYPE_BRICKATTACK)
		{
			if (e->t > 0 && e->t <= 1.0f)
				coEvents.push_back(e);
			else
				delete e;
		}
		else if (this->type == OBJECT_TYPE_MUSHROOM || this->type == OBJECT_TYPE_FLYGOOMBA)
		{
			if (e->t > 0 && e->t <= 1.0f)
			{
				if (coObjects->at(i)->type == OBJECT_TYPE_BRICK)			// avoid collision with mario and environment
					coEvents.push_back(e);
				else
					delete e;
			}
			else
				delete e;
		}
		else
		{
			if (e->t > 0 && e->t <= 1.0f)
			{
				if ((coObjects->at(i)->type == OBJECT_TYPE_PLANTFIREBALL) || (coObjects->at(i)->type == OBJECT_TYPE_FLOWER) || (coObjects->at(i)->type == OBJECT_TYPE_LEAF)  || (coObjects->at(i)->type == OBJECT_TYPE_COIN) || (coObjects->at(i)->type == OBJECT_TYPE_FIREBALL) || (coObjects->at(i)->type == OBJECT_TYPE_MUSHROOM))			// avoid collision with mario and environment
					delete e;
				else
					coEvents.push_back(e);
			}
			else
				delete e;
		}

	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CGameObject::FilterCollision(
	vector<LPCOLLISIONEVENT>& coEvents,
	vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	float& nx, float& ny, float& rdx, float& rdy)
{
	if (this->type == OBJECT_TYPE_MARIO)	// Mario
	{
		min_tx = 1.0f;
		min_ty = 1.0f;
		nx = 0.0f;
		ny = 0.0f;
		int min_ix = -1;
		int min_iy = -1;
		coEventsResult.clear();
		for (UINT i = 0; i < coEvents.size(); i++)
		{
			LPCOLLISIONEVENT c = coEvents[i];
			if (c->t < 1.0f && c->nx != 0) {
				if (c->t < min_tx)
				{
					min_tx = c->t;
					nx = c->nx;
					rdx = c->dx;
				}
				min_ix = i;
				coEventsResult.push_back(coEvents[min_ix]);
			}
			if (c->t < 1.0f && c->ny != 0) {
				if (c->t < min_ty)
				{
					min_ty = c->t;
					ny = c->ny;
					rdy = c->dy;
				}
				min_iy = i;
				coEventsResult.push_back(coEvents[min_iy]);
			}
		}
	}
	else
	{
		min_tx = 1.0f;
		min_ty = 1.0f;
		int min_ix = -1;
		int min_iy = -1;
		nx = 0.0f;
		ny = 0.0f;
		coEventsResult.clear();
		for (UINT i = 0; i < coEvents.size(); i++)
		{
			LPCOLLISIONEVENT c = coEvents[i];

			if (c->t < min_tx && c->nx != 0) {
				min_tx = c->t; nx = c->nx; min_ix = i; rdx = c->dx;
			}
			if (c->t < min_ty && c->ny != 0) {
				min_ty = c->t; ny = c->ny; min_iy = i; rdy = c->dy;
			}
		}
		if (min_ix >= 0)
			coEventsResult.push_back(coEvents[min_ix]);
		if (min_iy >= 0)
			coEventsResult.push_back(coEvents[min_iy]);
	}
}

bool CGameObject::CheckAABB(CGameObject* object)
{
	float l, t, r, b;
	float l1, t1, r1, b1;
	this->GetBoundingBox(l, t, r, b);
	object->GetBoundingBox(l1, t1, r1, b1);

	return !(r < l1 || l > r1 || t > b1 || b < t1);
}


void CGameObject::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	CGame::GetInstance()->Draw(x, y, bbox, rect.left, rect.top, rect.right, rect.bottom, 32);
}


CGameObject::~CGameObject()
{

}