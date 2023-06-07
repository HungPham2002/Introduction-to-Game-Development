#include <iostream>
#include <fstream>
#include "WorldMapScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "Game.h"
#include <cmath> 

using namespace std;

CWorldMapScene::CWorldMapScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	board = NULL;
	player = NULL;
	speedBar = NULL;
	numCoin.clear();
	numLive.clear();
	numScore.clear();
	numTime.clear();
	itemList.clear();
	isWaiting = 1;
	TimeWaitToScene = DWORD(GetTickCount64());
	key_handler = new CWorldMapSceneKeyHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_TEXTURES 2
#define SCENE_SECTION_SPRITES 3
#define SCENE_SECTION_ANIMATIONS 4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS	6

#define OBJECT_TYPE_MARIO	0
#define OBJECT_TYPE_OBJECT	1
#define OBJECT_TYPE_ENVIRONMENT 2
#define OBJECT_TYPE_BOARD 16
#define OBJECT_TYPE_NUMBER				22
#define OBJECT_TYPE_SPEEDBAR			23
#define OBJECT_TYPE_ITEM				24

#define MAX_SCENE_LINE 1024


void CWorldMapScene::_ParseSection_TEXTURES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 5) return; // skip invalid lines

	int texID = atoi(tokens[0].c_str());
	wstring path = ToWSTR(tokens[1]);
	int R = atoi(tokens[2].c_str());
	int G = atoi(tokens[3].c_str());
	int B = atoi(tokens[4].c_str());

	CTextures::GetInstance()->Add(texID, path.c_str(), D3DCOLOR_XRGB(R, G, B));
}

void CWorldMapScene::_ParseSection_SPRITES(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 6) return; // skip invalid lines

	int ID = atoi(tokens[0].c_str());
	int l = atoi(tokens[1].c_str());
	int t = atoi(tokens[2].c_str());
	int r = atoi(tokens[3].c_str());
	int b = atoi(tokens[4].c_str());
	int texID = atoi(tokens[5].c_str());

	LPDIRECT3DTEXTURE9 tex = CTextures::GetInstance()->Get(texID);
	if (tex == NULL)
	{
		DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
		return;
	}

	CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CWorldMapScene::_ParseSection_ANIMATIONS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines - an animation must at least has 1 frame and 1 frame time

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	LPANIMATION ani = new CAnimation();

	int ani_id = atoi(tokens[0].c_str());
	for (int i = 1; i < int(tokens.size()); i += 2)	// why i+=2 ?  sprite_id | frame_time  
	{
		int sprite_id = atoi(tokens[i].c_str());
		int frame_time = atoi(tokens[i + 1].c_str());
		ani->Add(sprite_id, frame_time);
	}

	CAnimations::GetInstance()->Add(ani_id, ani);
}

void CWorldMapScene::_ParseSection_ANIMATION_SETS(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 2) return; // skip invalid lines - an animation set must at least id and one animation id

	int ani_set_id = atoi(tokens[0].c_str());

	LPANIMATION_SET s = new CAnimationSet();

	CAnimations* animations = CAnimations::GetInstance();

	for (int i = 1; i < int(tokens.size()); i++)
	{
		int ani_id = atoi(tokens[i].c_str());

		LPANIMATION ani = animations->Get(ani_id);
		s->push_back(ani);
	}

	CAnimationSets::GetInstance()->Add(ani_set_id, s);
}

/*
	Parse a line in section [OBJECTS]
*/
void CWorldMapScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = float(atof(tokens[1].c_str()));
	float y = float(atof(tokens[2].c_str()));
	int ani_set_id = atoi(tokens[3].c_str());
	int object_setting = 0;
	float xSwitch = 0;
	float ySwitch = 0;

	if (tokens.size() == 5)
		object_setting = atoi(tokens[4].c_str());
	else if (tokens.size() == 6)
	{
		xSwitch = float(atof(tokens[4].c_str()));
		ySwitch = float(atof(tokens[5].c_str()));
	}

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new CMiniMario(x, y);
		player = (CMiniMario*)obj;
		DebugOut(L"[INFO] Player object created!\n");
		DebugOut(L"[INFO] obj_type = %d\n", object_type);
		DebugOut(L"[INFO] x = %f\n", x);
		DebugOut(L"[INFO] y = %f\n", y);
		break;
	case OBJECT_TYPE_OBJECT: obj = new CObject(); break;
		break;
	case OBJECT_TYPE_ENVIRONMENT: obj = new CEnvironment(); break;
		break;
	case OBJECT_TYPE_BOARD:
		obj = new CBoard();
		board = (CBoard*)obj;
		break;
	case OBJECT_TYPE_NUMBER:

		obj = new CNumber();
		if (object_setting == 0)
			numCoin.push_back((CNumber*)obj);
		else if (object_setting == 1)
			numTime.push_back((CNumber*)obj);
		else if (object_setting == 2)
			numScore.push_back((CNumber*)obj);
		else if (object_setting == 3)
			numLive.push_back((CNumber*)obj);
		obj->type = OBJECT_TYPE_NUMBER;
		break;
	case OBJECT_TYPE_SPEEDBAR:
		obj = new CSpeedBar();
		speedBar = (CSpeedBar*)obj;
		obj->type = OBJECT_TYPE_SPEEDBAR;
		break;
	case OBJECT_TYPE_ITEM:
		if (itemList.size() == 3)
			return;
		obj = new CItem();
		itemList.push_back((CItem*)obj);
		obj->type = OBJECT_TYPE_ITEM;
		break;
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
	objects.push_back(obj);
}

void CWorldMapScene::Load()
{
	CGame* game = CGame::GetInstance();
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);

	ifstream f;
	f.open(sceneFilePath);

	// current resource section flag
	int section = SCENE_SECTION_UNKNOWN;

	char str[MAX_SCENE_LINE];
	while (f.getline(str, MAX_SCENE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[TEXTURES]") { section = SCENE_SECTION_TEXTURES; continue; }
		if (line == "[SPRITES]") {
			section = SCENE_SECTION_SPRITES; continue;
		}
		if (line == "[ANIMATIONS]") {
			section = SCENE_SECTION_ANIMATIONS; continue;
		}
		if (line == "[ANIMATION_SETS]") {
			section = SCENE_SECTION_ANIMATION_SETS; continue;
		}
		if (line == "[OBJECTS]") {
			section = SCENE_SECTION_OBJECTS; continue;
		}
		if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_TEXTURES: _ParseSection_TEXTURES(line); break;
		case SCENE_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
		case SCENE_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
		case SCENE_SECTION_ANIMATION_SETS: _ParseSection_ANIMATION_SETS(line); break;
		case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();

	//CTextures::GetInstance()->Add(ID_TEX_BBOX, L"textures\\bbox.png", D3DCOLOR_XRGB(255, 255, 255));

	DebugOut(L"[INFO] Done loading scene resources %s\n", sceneFilePath);
	player->SetLevel(game->GetLevel());
	player->SetPosition(game->GetMiniMarioPositionX(), game->GetMiniMarioPositionY());
}

void CWorldMapScene::Update(DWORD dt)
{
	// We know that Mario is the first object in the list hence we won't add him into the colliable object list
	// TO-DO: This is a "dirty" way, need a more organized way 

	vector<LPGAMEOBJECT> coObjects;
	CGame* game = CGame::GetInstance();
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (objects[i]->type == OBJECT_TYPE_MARIO)
			continue;
		coObjects.push_back(objects[i]);
	}

	for (size_t i = 0; i < objects.size(); i++)
	{
		objects[i]->Update(dt, &coObjects);
	}

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	TimeWaitToLoad();

	float xCoin =  180.0f;
	vector<int> temp;
	temp = getNum(game->GetCoin());
	for (int i = 0, j = 0; i < int(numCoin.size()); i++, j++)
	{
		if (j < int(temp.size()))
			numCoin[i]->Update(xCoin, 219.0f, temp[j]);
		else
			numCoin[i]->Update(xCoin, 219.0f, 0);
		xCoin -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetTime());
	float xTime =  180.0f;
	for (int i = numTime.size() - 1, j = 0; i >= 0; i--, j++)
	{
		if (j < int(temp.size()))
			numTime[i]->Update(xTime, 227.0f, temp[j]);
		else
			numTime[i]->Update(xTime, 227.0f, 0);
		xTime -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetLive());
	float xLive =  77.0f;
	for (int i = numLive.size() - 1, j = 0; i >= 0; i--, j++)
	{
		if (j < int(temp.size()))
			numLive[i]->Update(xLive, 227.0f, temp[j]);
		else
			numLive[i]->Update(xLive, 227.0f, 0);
		xLive -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetScore());
	float xScore =  140.0f;
	for (int i = 0, j = 0; i < int(numScore.size()); i++, j++)
	{
		if (j < int(temp.size()))
			numScore[i]->Update(xScore, 227.0f, temp[j]);
		else
			numScore[i]->Update(xScore, 227.0f, 0);
		xScore -= NUMBER_WIDTH;
	}

	//float xSpeedBar = camx + 92.0f;
	//speedBar->Update(xSpeedBar, board->y + 17, player->speedStack);

	temp.clear();
	temp = game->GetItemList();
	float xItemList =  207.0f;
	for (int i = 0; i < int(itemList.size()); i++)
	{
		itemList[i]->SetState(temp[i]);
		itemList[i]->SetPosition(xItemList, board->y + 16);
		xItemList += 24.0f;
	}
	temp.clear();
	//DebugOut(L"time = %d \n", game->GetTime());
	// Update camera to follow mario
	float cx, cy;
	float camx, camy;
	player->GetPosition(cx, cy);
	game->GetCamPos(camx, camy);

	CGame::GetInstance()->SetCamPos(0.0f, 0.0f); // set Cam when game start
}

void CWorldMapScene::Render()
{
	if (isWaiting)
		return;
	for (int i = 0; i < int(objects.size()); i++)
		objects[i]->Render();
}

/*
	Unload current scene
*/
void CWorldMapScene::Unload()
{
	for (int i = 0; i < int(objects.size()); i++)
		delete objects[i];

	objects.clear();
	player = NULL;
	board = NULL;
	numCoin.clear();
	numLive.clear();
	numScore.clear();
	numTime.clear();
	itemList.clear();

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CWorldMapSceneKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMiniMario* mario = ((CWorldMapScene*)scence)->GetPlayer();
	CGame* game = CGame::GetInstance();
	switch (KeyCode)
	{
	case DIK_1:
		mario->SetLevel(MINIMARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		mario->SetLevel(MINIMARIO_LEVEL_BIG);
		break;
	case DIK_3:
		mario->SetLevel(MINIMARIO_LEVEL_FIRE);
		break;
	case DIK_4:
		mario->SetLevel(MINIMARIO_LEVEL_RACOON);
		break;
	case DIK_RIGHT:
		if (mario->GetState() == MINIMARIO_STATE_IDLE)
			mario->SetState(MINIMARIO_STATE_WALKING_RIGHT);
		break;
	case DIK_LEFT:
		if (mario->GetState() == MINIMARIO_STATE_IDLE)
			mario->SetState(MINIMARIO_STATE_WALKING_LEFT);
		break;
	case DIK_DOWN:
		if (mario->GetState() == MINIMARIO_STATE_IDLE)
			mario->SetState(MINIMARIO_STATE_WALKING_DOWN);
		break;
	case DIK_UP:
		if (mario->GetState() == MINIMARIO_STATE_IDLE)
			mario->SetState(MINIMARIO_STATE_WALKING_UP);
		break;
	case DIK_S:
	{
		if (mario->x > 94.0f && mario->x < 101.0f && mario->y > 29.0f && mario->y < 35.0f)
		{
			game->SetLevel(mario->getLevel());
			game->SetTime(300);
			CGame::GetInstance()->SwitchScene(2);

		}
		else if (mario->x > 190.0f && mario->x < 197.0f && mario->y > 60.0f && mario->y < 67.0f)
		{
			game->SetLevel(mario->getLevel());
			game->SetTime(300);
			CGame::GetInstance()->SwitchScene(3);
		}
	}
		break;
	}
}

void CWorldMapSceneKeyHandler::OnKeyUp(int KeyCode)
{

}

void CWorldMapSceneKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMiniMario* mario = ((CWorldMapScene*)scence)->GetPlayer();

}

vector<int> CWorldMapScene::getNum(int number)
{
	vector<int> result;
	int odd;
	if (number == 0)
	{
		result.push_back(0);
	}
	else
	{
		while (number > 0)
		{
			odd = number % 10;
			result.push_back(odd);
			number = number / 10;
		}
	}
	return result;
}

void CWorldMapScene::TimeWaitToLoad()
{
	if (GetTickCount64() - TimeWaitToScene > ONE_SECS * 1)
	{
		TimeWaitToScene = 0;
		isWaiting = 0;
	}
}
