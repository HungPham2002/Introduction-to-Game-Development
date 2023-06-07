#pragma once
#include "Game.h" 
#include "GameObject.h"
#include "Game.h"
#include "Brick.h"
#include "GameClearBoard.h"
#include "Mario.h"
#include "PButton.h"
#include "BoomerangBros.h"
#include "EndPointItem.h"
#include "PiranhaPlant.h"
#include "MoveBar.h"
#include "QBrick.h"
#include "BrokenBrick.h"



using namespace std;


#define GRID_CELL_WIDTH 83
#define GRID_CELL_HEIGHT 73

#define GRID_CELL_MAX_ROW 40
#define GRID_CELL_MAX_COLUMN 40 

#define MAXSCENELINE 1024

class Grid
{
private:
	vector<CGameObject*> cells[GRID_CELL_MAX_ROW][GRID_CELL_MAX_COLUMN];
	char* filepath;

public:
	Grid();
	~Grid();
	void SetFile(char* str);
	void HandleCell(int top, int left, int right, int bottom);
	void loadGrid(CMario * player, vector<CQBrick*>& qbrick, vector<CBrokenBrick*>& bbrick, vector<CMoveBar*>& bar, vector<CEndPointItem*>& item);
	void GetListObject(vector<CGameObject*>& ListObj);

};
