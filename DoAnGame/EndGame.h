#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scence.h"
#include "GameObject.h"
#include "MiniMario.h"
#include "Object.h"
#include "Environment.h"
#include "Board.h"
#include "Number.h"
#include "SpeedBar.h"
#include "Item.h"
#include "Tag.h"

#define ONE_SECS 1000


class CEndGame : public CScene
{
protected:
	CBoard* board = NULL;
	CTag* tag = NULL;
	vector<LPGAMEOBJECT> objects;

	// game play
	vector<CNumber*> numCoin;
	vector<CNumber*> numTime;
	vector<CNumber*> numScore;
	vector<CNumber*> numLive;
	CSpeedBar* speedBar;
	vector<CItem*> itemList;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);

public:

	CEndGame(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();
	vector<int> getNum(int number);

	CTag* GetTag() { return tag; }

	//friend class CPlayScenceKeyHandler;
};

class CEndGameKeyHandler : public CScenceKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CEndGameKeyHandler(CScene* s) :CScenceKeyHandler(s) {};
};

