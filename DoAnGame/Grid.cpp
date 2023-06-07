#include "Grid.h"
#include <iostream>
#include <fstream>
#include <cmath> 
#include "Utils.h"
#include "Mario.h"

#define OBJECT_TYPE_MARIO				0
#define OBJECT_TYPE_BRICK				1
#define OBJECT_TYPE_GOOMBA				2
#define OBJECT_TYPE_KOOPAS				3
#define OBJECT_TYPE_ENVIRONMENT			4
#define OBJECT_TYPE_UPSIDEBRICK			5
#define OBJECT_TYPE_COIN				6
#define OBJECT_TYPE_QBRICK				7
#define OBJECT_TYPE_FIREBALL			8
#define OBJECT_TYPE_FLYGOOMBA			9
#define OBJECT_TYPE_FLYKOOPAS			10
#define OBJECT_TYPE_PLANT				11
#define OBJECT_TYPE_PIRANHAPLANT		12
#define OBJECT_TYPE_SWITCH				13
#define OBJECT_TYPE_BROKENBRICK			14
#define OBJECT_TYPE_PBUTTON				15
#define OBJECT_TYPE_BOARD				16
#define OBJECT_TYPE_PLANTFIREBALL		17
#define OBJECT_TYPE_MUSHROOM			18
#define OBJECT_TYPE_LEAF				19
#define OBJECT_TYPE_ENDPOINTITEM		20
#define OBJECT_TYPE_GAMECLEARBOARD		21
#define OBJECT_TYPE_NUMBER				22
#define OBJECT_TYPE_SPEEDBAR			23
#define OBJECT_TYPE_ITEM				24
#define OBJECT_TYPE_BOOMERANGBROS		25
#define OBJECT_TYPE_BOOMERANG			26
#define OBJECT_TYPE_MOVEBAR				27
#define OBJECT_TYPE_PORTAL				50

Grid::Grid()
{
}

Grid::~Grid()
{
	for (int i = 0; i < GRID_CELL_MAX_ROW; i++)
		for (int j = 0; j < GRID_CELL_MAX_COLUMN; j++)
		{
			cells[i][j].clear();
		}
}
 
void Grid::SetFile(char* str)
{
	filepath = str;
}

void Grid::loadGrid(CMario* player, vector<CQBrick*>& qbrick, vector<CBrokenBrick*>& bbrick, vector<CMoveBar*>& bar, vector<CEndPointItem*>& item)
{
	for (int i = 0; i < GRID_CELL_MAX_ROW; i++)
		for (int j = 0; j < GRID_CELL_MAX_COLUMN; j++)
		{
			cells[i][j].clear();
		}

	ifstream f;
	f.open(filepath);
	char str[MAXSCENELINE];
	int id = 1;
	while (f.getline(str, MAXSCENELINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	
		vector<string> tokens = split(line);
		if (tokens.size() < 3) continue; // skip invalid lines 
		int object_type = atoi(tokens[0].c_str());
		float x = float(atof(tokens[1].c_str()));
		float y = float(atof(tokens[2].c_str()));
		int ani_set_id = atoi(tokens[3].c_str());
		int j = atoi(tokens[4].c_str());
		int i = atoi(tokens[5].c_str());


		CAnimationSets* animation_sets = CAnimationSets::GetInstance();

		CGameObject* obj = NULL;

		switch (object_type)
		{
		case OBJECT_TYPE_GOOMBA:
		{
			obj = new CGoomba();
			obj->SetInitialX(x);
			obj->SetInitialY(y);
			obj->type = OBJECT_TYPE_GOOMBA;
		}
			break;
		case OBJECT_TYPE_KOOPAS:
		{
			int type = atoi(tokens[6].c_str());
			obj = new CKoopas(type);
			obj->SetInitialX(x);
			obj->SetInitialY(y);
			obj->type = OBJECT_TYPE_KOOPAS;
		}
			break;
		case OBJECT_TYPE_FLYGOOMBA:
		{
			obj = new CFlyGoomba();
			obj->SetInitialX(x);
			obj->SetInitialY(y);
			obj->type = OBJECT_TYPE_FLYGOOMBA;
		}
			break;
		case OBJECT_TYPE_FLYKOOPAS:
		{
			float yMin = float(atof(tokens[6].c_str()));
			float yMax = float(atof(tokens[7].c_str()));
			int type = atoi(tokens[8].c_str());
			obj = new CFlyKoopas(yMin, yMax, type);
			obj->SetInitialX(x);
			obj->SetInitialY(y);
			obj->type = OBJECT_TYPE_FLYKOOPAS;
		}
			break;
		case OBJECT_TYPE_BRICK:
			obj = new CBrick();
			obj->type = OBJECT_TYPE_BRICK;
			break;
		case OBJECT_TYPE_UPSIDEBRICK:
			obj = new CUpsideBrick();
			obj->type = OBJECT_TYPE_BRICK;
			break;
		case OBJECT_TYPE_COIN:
			obj = new CCoin();
			obj->type = OBJECT_TYPE_COIN;
			break;
		case OBJECT_TYPE_QBRICK:
		{
			float setting1 = float(atof(tokens[6].c_str()));
			float setting2 = float(atof(tokens[7].c_str()));
			obj = new CQBrick(player, int(setting1), int(setting2), y);
			obj->type = OBJECT_TYPE_BRICK;
			qbrick.push_back((CQBrick*)obj);
		}
		break;
		case OBJECT_TYPE_ENDPOINTITEM:
			obj = new CEndPointItem();
			obj->type = OBJECT_TYPE_ENDPOINTITEM;
			item.push_back((CEndPointItem*)obj);
			break;
		case OBJECT_TYPE_SWITCH:
		{
			float setting1 = float(atof(tokens[6].c_str()));
			float setting2 = float(atof(tokens[7].c_str()));
			obj = new CSwitch(setting1, setting2);
			obj->type = OBJECT_TYPE_BRICK;
		}
		break;
		case OBJECT_TYPE_BROKENBRICK:
			obj = new CBrokenBrick();
			bbrick.push_back((CBrokenBrick*)obj);
			obj->type = OBJECT_TYPE_BRICK;
			break;
		case OBJECT_TYPE_MOVEBAR:
			obj = new CMoveBar();
			bar.push_back((CMoveBar*)obj);
			obj->type = OBJECT_TYPE_BRICK;
			break;
		case OBJECT_TYPE_PORTAL:
		{
			float r = float(atof(tokens[6].c_str()));
			float b = float(atof(tokens[7].c_str()));
			int scene_id = atoi(tokens[8].c_str());
			obj = new CPortal(x, y, r, b, scene_id);
			obj->type = OBJECT_TYPE_PORTAL;
		}
		break;
		}
		obj->SetPosition(x, y);
		obj->SetId(id);
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
		cells[i][j].push_back(obj);
		id++;
	}
	f.close();


}

void Grid::GetListObject(vector<CGameObject*>& ListObj)
{
	ListObj.clear();
	CGame* game = CGame::GetInstance();
	float camx;
	float camy;
	game->GetCamPos(camx, camy);

	int bottom = (int)((camy + game->GetScreenHeight()) / GRID_CELL_HEIGHT);
	int top = (int)((camy) / GRID_CELL_HEIGHT);

	int left = (int)((camx) / GRID_CELL_WIDTH);
	int right = (int)((camx + game->GetScreenWidth()) / GRID_CELL_WIDTH);

	HandleCell(top, left, right, bottom);

	for (int i = top; i <= bottom; i++)
		for (int j = left; j <= right; j++)
			for (int temp = 0; temp < int(cells[i][j].size()); temp++)
				ListObj.push_back(cells[i][j].at(temp));

}

void Grid::HandleCell(int top, int left, int right, int bottom)
{
	for(int i = top; i <= bottom; i++)
		for (int j = left; j <= right; j++)
			if (cells[i][j].size() > 0)
			{
				for (int temp = 0; temp < int(cells[i][j].size()); temp++)
				{
					if (cells[i][j].at(temp)->type == OBJECT_TYPE_KOOPAS || cells[i][j].at(temp)->type == OBJECT_TYPE_FLYKOOPAS ||  cells[i][j].at(temp)->type == OBJECT_TYPE_GOOMBA || cells[i][j].at(temp)->type == OBJECT_TYPE_FLYGOOMBA)
					{
						int currenti = (int)((cells[i][j].at(temp)->y) / GRID_CELL_HEIGHT);
						int currentj = (int)((cells[i][j].at(temp)->x) / GRID_CELL_WIDTH);
						if (currenti != i || currentj != j)
						{
							cells[currenti][currentj].push_back(cells[i][j].at(temp));
							cells[i][j].erase(cells[i][j].begin() + temp);
						} 
					}
				}
			}
}
