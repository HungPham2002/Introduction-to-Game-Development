#include <iostream>
#include <fstream>
#include "PlayScence.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include <cmath> 
#include <stdio.h>

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) :
	CScene(id, filePath)
{
	this->id = id;
	grid = NULL;
	player = NULL;
	bros = NULL;
	speedBar = NULL;
	gameclearboard = NULL;
	Dtime = NULL;
	board = NULL;
	button = NULL;
	isWaiting = 1;
	TimeBrokenBrickBackToBrick = NULL;
	TimeWaitToScene = DWORD(GetTickCount64());
	key_handler = new CPlayScenceKeyHandler(this);
}

/*
	Load scene resources from scene file (textures, sprites, animations and objects)
	See scene1.txt, scene2.txt for detail format specification
*/

#define SCENE_SECTION_UNKNOWN			-1
#define SCENE_SECTION_TEXTURES			2
#define SCENE_SECTION_SPRITES			3
#define SCENE_SECTION_ANIMATIONS		4
#define SCENE_SECTION_ANIMATION_SETS	5
#define SCENE_SECTION_OBJECTS			6

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
#define OBJECT_TYPE_FLOWER				28
#define OBJECT_TYPE_MUSHROOM_GREEN		29
#define OBJECT_TYPE_PORTAL				50

#define MAX_SCENE_LINE 1024


void CPlayScene::_ParseSection_TEXTURES(string line)
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

void CPlayScene::_ParseSection_SPRITES(string line)
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

void CPlayScene::_ParseSection_ANIMATIONS(string line)
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

void CPlayScene::_ParseSection_ANIMATION_SETS(string line)
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
void CPlayScene::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);

	//DebugOut(L"--> %s\n",ToWSTR(line).c_str());

	if (tokens.size() < 3) return; // skip invalid lines - an object set must have at least id, x, y

	int object_type = atoi(tokens[0].c_str());
	float x = float(atof(tokens[1].c_str()));
	float y = float(atof(tokens[2].c_str()));
	int ani_set_id = atoi(tokens[3].c_str());


	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_PLANT:
		obj = new CPlant(player, y);
		obj->type = OBJECT_TYPE_PLANT;
		listEnemies.push_back((CPlant*)obj);
		break;
	case OBJECT_TYPE_PIRANHAPLANT:
		obj = new CPiranhaPlant(player, y);
		plant.push_back((CPiranhaPlant*)obj);
		obj->type = OBJECT_TYPE_PIRANHAPLANT;
		listEnemies.push_back((CPiranhaPlant*)obj);
		break;
	case OBJECT_TYPE_BOOMERANGBROS:
	{
		float setting1 = float(atof(tokens[4].c_str()));
		float setting2 = float(atof(tokens[5].c_str()));
		obj = new CBoomerangBros(setting1, setting2);
		obj->type = OBJECT_TYPE_BOOMERANGBROS;
		bros = (CBoomerangBros*)obj;
		listEnemies.push_back((CBoomerangBros*)obj);
	}
	break;
	case OBJECT_TYPE_MARIO:
		if (player != NULL)
		{
			DebugOut(L"[ERROR] MARIO object was created before!\n");
			return;
		}
		obj = new CMario(x, y);
		obj->type = OBJECT_TYPE_MARIO;
		player = (CMario*)obj;
		DebugOut(L"[INFO] Player object created!\n");
		break;
	case OBJECT_TYPE_ENVIRONMENT:
		obj = new CEnvironment();
		obj->type = OBJECT_TYPE_ENVIRONMENT;
		background.push_back((CEnvironment*)obj);
		break;
	case OBJECT_TYPE_BOARD:
		obj = new CBoard();
		board = (CBoard*)obj;
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_NUMBER:
	{
		int object_setting = atoi(tokens[4].c_str());
		obj = new CNumber();
		if (object_setting == 0)
			numCoin.push_back((CNumber*)obj);
		else if (object_setting == 1)
			numTime.push_back((CNumber*)obj);
		else if (object_setting == 2)
			numScore.push_back((CNumber*)obj);
		else if (object_setting == 3)
			numLive.push_back((CNumber*)obj);
		obj->type = OBJECT_TYPE_BOARD;
	}
		break;
	case OBJECT_TYPE_GAMECLEARBOARD:
		obj = new CGameClearBoard();
		gameclearboard = (CGameClearBoard*)obj;
		break;
	case OBJECT_TYPE_SPEEDBAR:
		obj = new CSpeedBar();
		speedBar = (CSpeedBar*)obj;
		obj->type = OBJECT_TYPE_BOARD;
		break;
	case OBJECT_TYPE_ITEM:
		obj = new CItem();
		itemList.push_back((CItem*)obj);
		obj->type = OBJECT_TYPE_BOARD;
		break;
	default:
		//DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}

	// General object setup
	obj->SetPosition(x, y);

	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);

	obj->SetAnimationSet(ani_set);
}

void CPlayScene::Load()
{
	CGame* game = CGame::GetInstance();
	DebugOut(L"[INFO] Start loading scene resources from : %s \n", sceneFilePath);
	//grid.loadGrid(player);
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

	// grid initialize
	grid = new Grid();

	// set file grid
	if (this->id == MAP_11)
		grid->SetFile("grid_map1-1.txt");
	else if (this->id == MAP_14)
		grid->SetFile("grid_map1-4.txt");

	// LoadGrid
	grid->loadGrid( player, qbrick, bbrick, bar, item);
	player->SetLevel(game->GetLevel());
}

void CPlayScene::Update(DWORD dt)
{

	TimeLapse();					// reduce time 1 sec
	if (isWaiting)
		return;

	vector<LPGAMEOBJECT> coObjects;
	CGame* game = CGame::GetInstance();
	grid->GetListObject(listObjects);

	// delete create object finished
	for (int i = 0; i < int(createObjects.size()); i++)
	{
		if (createObjects[i]->isFinish == 1)
			createObjects.erase(createObjects.begin() + i);
	}

	// if player is transforming or die then frozen sceene
	if (player->transform || player->transformRacoon || player->GetState() == MARIO_STATE_DIE)	// frozen scene
	{
		for (size_t i = 0; i < listObjects.size(); i++)
		{
			coObjects.push_back(listObjects[i]);
		}
		for (size_t i = 0; i < createObjects.size(); i++)
		{
			coObjects.push_back(createObjects[i]);
		}
		for (size_t i = 0; i < listEnemies.size(); i++)
		{
			coObjects.push_back(listEnemies[i]);
		}
		player->Update(dt, &coObjects);
		return;
	}

	// out of time then die
	if (game->GetTime() < 0)
		player->SetState(MARIO_STATE_DIE);

	// Draw fireball
	if (player->fireball)			
	{
		int temp = 0;
		player->fireball -= 1;
		for (int i = 0; i < int(createObjects.size()); i++)
		{
			if (createObjects[i]->type == OBJECT_TYPE_FIREBALL && createObjects[i]->isFinish == 0)
				temp++;
		}
		if (temp < 2)			//max fire ball = 2
		{
			createObjects.push_back(player->NewFireBall());
			player->StartThrowing();
		}

		else;
	}

	//create tail attack
	if (player->tail)
	{
		player->tail -= 1;
		createObjects.push_back(player->TailAttack());
	}

	// show gameclearboard if mario hit the endpoint item
	if (gameclearboard != NULL)
		if (gameclearboard->GetState() == BOARD_STATE_EMPTY)
			if (item[0] != NULL)
				if (item[0]->sparkling == 1)
					gameclearboard->SetState(BOARD_STATE_STAR);
				else if (item[0]->sparkling == 2)
					gameclearboard->SetState(BOARD_STATE_MUSHROOM);
				else if (item[0]->sparkling == 3)
					gameclearboard->SetState(BOARD_STATE_FLOWER);
				else;

	// boomerang bros throws boomerang
	if (bros != NULL)
	{
		if (bros->boomerang)
		{
			bros->boomerang -= 1;
			createObjects.push_back(bros->NewBoomerang());
		}
	}

	// plant create fireball attack
	for (int i = 0; i < int(plant.size()); i++)
	{
		if (plant[i]->fireball)						// Draw fireball
		{
			plant[i]->fireball -= 1;
			createObjects.push_back(plant[i]->NewFireBall());
		}
	}

	// question brick trigger show items
	for (int i = 0; i < int(qbrick.size()); i++)
	{
		if (qbrick[i]->trigger)						// show item
		{
			qbrick[i]->trigger -= 1;
			CGameObject* obj;
			obj = qbrick[i]->ShowItem();
			createObjects.push_back(obj);
			if (obj->type == OBJECT_TYPE_PBUTTON)
			{
				button = (CPButton*)obj;
			}
		}
		if (qbrick[i]->attack)						// Tail Attack
		{
			qbrick[i]->attack -= 1;
			createObjects.push_back(qbrick[i]->Attack());
		}
	}


	for (int i = 0; i < int(bbrick.size()); i++)
	{
		// broken bricks turn into pieces
		if (bbrick[i]->trigger)						// Brick fragment
		{
			bbrick[i]->trigger -= 1;
			vector<CGameObject*> temp = bbrick[i]->Broken();
			createObjects.insert(createObjects.end(), temp.begin(), temp.end());
		}

		// after 10 seconds then turn back to brokenbricks
		if (bbrick[i]->GetState() == BROKENBRICK_STATE_COIN)
		{
			if (isBrokenBrickBackToBrick)
			{
				bbrick[i]->SetState(BROKENBRICK_STATE_BRICK);
				bbrick[i]->type = OBJECT_TYPE_BROKENBRICK;
			}
		}

		if (bbrick[i]->attack)						// Brick Attack
		{
			bbrick[i]->attack -= 1;
			createObjects.push_back(bbrick[i]->Attack());
		}
	}

	// if stomped on P button then turn brokenbricks to coin
	if (button != NULL)
	{
		if (button->trigger)
		{
			button->trigger -= 1;
			for (int i = 0; i < int(bbrick.size()); i++)
			{
				bbrick[i]->SetState(BROKENBRICK_STATE_COIN); // turn brokenbrick into coin
				bbrick[i]->type = OBJECT_TYPE_COIN;
				StartCountTimeBackToBrick();
			}
		}
	}

	// pushback object being collided
	for (size_t i = 0; i < listObjects.size(); i++)
	{
		coObjects.push_back(listObjects[i]);
	}
	for (size_t i = 0; i < createObjects.size(); i++)
	{
		coObjects.push_back(createObjects[i]);
	}
	for (size_t i = 0; i < listEnemies.size(); i++)
	{
		coObjects.push_back(listEnemies[i]);
	}

	// update object
	for (size_t i = 0; i < listObjects.size(); i++)
	{
		listObjects[i]->Update(dt, &coObjects);
	}
	for (size_t i = 0; i < createObjects.size(); i++)
	{
		createObjects[i]->Update(dt, &coObjects);
	}
	for (size_t i = 0; i < listEnemies.size(); i++)
	{
		listEnemies[i]->Update(dt, &coObjects);
	}

	// check mario is out of movebar or not
	IsOutOfMoveBar();

	// if player on air then none of move bar is stomped
	if (player->isFlying)
		ClearBarStomped();

	// update player
	player->Update(dt, &coObjects);

	// set AABB collision with enemies
	IsCollisionAABBWithEnemies();

	// set AABB collision with items
	IsCollisionAABBWithItems();

	//clear coObject
	coObjects.clear();

	// skip the rest if scene was already unloaded (Mario::Update might trigger PlayScene::Unload)
	if (player == NULL) return;

	if (player->GetState() == MARIO_STATE_DIE)
		return;


	float cx, cy;
	float camx, camy;
	player->GetPosition(cx, cy);
	UpdateCamera(cx, cy, id);			// Update camera
	game->GetCamPos(camx, camy);
	board->Update(camx, camy);		// Update Board follow player
	UpdateBoardInfo(camx);			// Update BoardInfo

	// set min location of player
	if (player->x < camx) 
		player->x = camx;

	// player hit the endpoint item and go through the screen
	if (player->x > camx + game->GetScreenWidth() - 30.0f && player->GetState() != MARIO_STATE_ENDGAME) 
		player->x = camx + game->GetScreenWidth() - 30.0f;
}

void CPlayScene::Render()
{
	// if on state waiting to screen then return
	if (isWaiting)
		return;

	// render background
	for (int i = 0; i < int(background.size()); i++)
		background[i]->Render();

	// render gameclearboard
	gameclearboard->Render();

	// render mario
	player->Render();

	// render flower or mushroom before question brick
	for (int i = 0; i < int(createObjects.size()); i++)
	{
		if (createObjects[i]->type != OBJECT_TYPE_MUSHROOM && createObjects[i]->type != OBJECT_TYPE_FLOWER && createObjects[i]->type != OBJECT_TYPE_MUSHROOM_GREEN)
			continue;
		createObjects[i]->Render();
	}

	// render plant before because it needs to "Underpipe"
	for (int i = 0; i < int(listEnemies.size()); i++)
	{
		if (listEnemies[i]->type != OBJECT_TYPE_PLANT && listEnemies[i]->type != OBJECT_TYPE_PIRANHAPLANT)
			continue;
		listEnemies[i]->Render();
	}
	// render objects
	for (int i = 0; i < int(listObjects.size()); i++)
		listObjects[i]->Render();
	// render list enenies
	for (int i = 0; i < int(listEnemies.size()); i++)
	{
		if (listEnemies[i]->type == OBJECT_TYPE_PLANT || listEnemies[i]->type == OBJECT_TYPE_PIRANHAPLANT)
			continue;
		listEnemies[i]->Render();
	}

	// render created objects like fireball of mario and piranhaplant, etc...
	for (int i = 0; i < int(createObjects.size()); i++)
	{
		if (createObjects[i]->type == OBJECT_TYPE_MUSHROOM || createObjects[i]->type == OBJECT_TYPE_FLOWER || createObjects[i]->type == OBJECT_TYPE_MUSHROOM_GREEN)
			continue;
		createObjects[i]->Render();
	}


	// Under bar render
	board->Render();
	for (int i = 0; i < int(numCoin.size()); i++)
		numCoin[i]->Render();
	for (int i = 0; i < int(numTime.size()); i++)
		numTime[i]->Render();
	for (int i = 0; i < int(numScore.size()); i++)
		numScore[i]->Render();
	for (int i = 0; i < int(numLive.size()); i++)
		numLive[i]->Render();
	for (int i = 0; i < int(itemList.size()); i++)
		itemList[i]->Render();
	speedBar->Render();

}

/*
	Unload current scene
*/
void CPlayScene::Unload()
{
	//for (int i = 0; i < int(background.size()); i++)
	//	delete background[i];
	//for (int i = 0; i < int(listObjects.size()); i++)
	//	delete listObjects[i];
	//for (int i = 0; i < int(createObjects.size()); i++)
	//	delete createObjects[i];
	//for (int i = 0; i < int(listEnemies.size()); i++)
	//	delete listEnemies[i];
	listEnemies.clear();
	listObjects.clear();
	createObjects.clear();
	background.clear();
	player = NULL;
	bros = NULL;
	board = NULL;
	grid = NULL;
	button = NULL;
	gameclearboard = NULL;
	item.clear();
	bar.clear();
	plant.clear();
	qbrick.clear();
	bbrick.clear();
	numCoin.clear();
	numLive.clear();
	numScore.clear();
	numTime.clear();
	itemList.clear();

	DebugOut(L"[INFO] Scene %s unloaded! \n", sceneFilePath);
}

void CPlayScenceKeyHandler::OnKeyDown(int KeyCode)
{
	//DebugOut(L"[INFO] KeyDown: %d\n", KeyCode);

	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	if (mario->GetState() == MARIO_STATE_DIE || mario->GetState() == MARIO_STATE_ENDGAME || mario->transform || mario->transformRacoon) return;
	switch (KeyCode)
	{
	case DIK_S:
		mario->isInMoveBar = 0;		// if jump then out of movebar
		if ((mario->getLevel() == MARIO_LEVEL_RACOON && mario->isFlying == 1 && mario->isRunning == 1 && mario->sliding == 1) || mario->flying)			//condition to fly
			mario->SetState(MARIO_RACOON_STATE_FLY);
		else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->isFlying == 1)
			mario->SetState(MARIO_RACOON_STATE_FALL);
		else if (mario->GetState() != MARIO_STATE_DUCK)
			mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_X:
		mario->isInMoveBar = 0;	// if jump then out of movebar
		if ((mario->getLevel() == MARIO_LEVEL_RACOON && mario->isFlying == 1 && mario->isRunning == 1 && mario->sliding == 1) || mario->flying)			//condition to fly
			mario->SetState(MARIO_RACOON_STATE_FLY);
		else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->isFlying == 1)
			mario->SetState(MARIO_RACOON_STATE_FALL);
		else if (mario->GetState() != MARIO_STATE_DUCK)
			mario->SetState(MARIO_STATE_JUMP_SHORT);
		break;
	case DIK_T:
		mario->SetPosition(138,480);
		break;
	case DIK_A:
		if (mario->getLevel() == MARIO_LEVEL_RACOON)
			mario->SetState(MARIO_RACOON_STATE_TAIL);
		else if (mario->getLevel() == MARIO_LEVEL_FIRE)
			mario->SetState(MARIO_FIRE_STATE_THROW);						
		break;
	case DIK_Z:
		if (mario->getLevel() == MARIO_LEVEL_RACOON)
			mario->SetState(MARIO_RACOON_STATE_TAIL);
		else if (mario->getLevel() == MARIO_LEVEL_FIRE)
			mario->SetState(MARIO_FIRE_STATE_THROW);
		break;
	case DIK_1:
		if (mario->getLevel() != MARIO_LEVEL_SMALL)
			CMario::ToSmall(mario->y);
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		if (mario->getLevel() == MARIO_LEVEL_SMALL)
			CMario::ToBig(mario->y);
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_3:
		if (mario->getLevel() == MARIO_LEVEL_SMALL)
			CMario::ToBig(mario->y);
		mario->SetLevel(MARIO_LEVEL_FIRE);
		break;
	case DIK_4:
		if (mario->getLevel() == MARIO_LEVEL_SMALL)
			CMario::ToBig(mario->y);
		mario->SetLevel(MARIO_LEVEL_RACOON);
		break;
	}
}

void CPlayScenceKeyHandler::OnKeyUp(int KeyCode)
{
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();
	switch (KeyCode)
	{
	case DIK_DOWN:
		if (mario->isFlying == 0 && mario->getLevel() != MARIO_LEVEL_SMALL)
		{
			if (mario->isDucking)
			{
				mario->isDucking = 0;
				mario->y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_BIG_DUCK_BBOX_HEIGHT);
			}
		}
		break;
	}

}

void CPlayScenceKeyHandler::KeyState(BYTE* states)
{
	CGame* game = CGame::GetInstance();
	CMario* mario = ((CPlayScene*)scence)->GetPlayer();

	// disable control key when Mario die 
	if (mario->GetState() == MARIO_STATE_DIE || mario->GetState() == MARIO_STATE_ENDGAME || mario->transform || mario->transformRacoon) return;

	if (game->IsKeyDown(DIK_A))
	{
		mario->canHold = 1;
		if (!mario->flying)
			mario->isRunning = 1;
		if (abs(mario->GetVx()) >= MARIO_RUNNING_SPEED)
		{
			mario->SetState(MARIO_STATE_SLIDE);
		}

	}
	else {
		mario->isRunning = 0;
		mario->sliding = 0;
		mario->canHold = 0;
		mario->holding = 0;
	}

	if (game->IsKeyDown(DIK_RIGHT))
	{
		if (!mario->isDucking)
		{
			if (mario->vx < 0 && mario->turning == 0)
				mario->SetState(MARIO_RACOON_STATE_TURN);
			else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->nx < 0)
			{
				mario->x = mario->x - (MARIO_RACOON_BBOX_WIDTH - MARIO_BIG_BBOX_WIDTH);
			}
			mario->SetState(MARIO_STATE_WALKING_RIGHT);
		}
	}
	else if (game->IsKeyDown(DIK_LEFT))
	{
		if (!mario->isDucking)
		{
			if (mario->vx > 0 && mario->turning == 0)
				mario->SetState(MARIO_RACOON_STATE_TURN);
			else if (mario->getLevel() == MARIO_LEVEL_RACOON && mario->nx > 0)
			{
				mario->x = mario->x + MARIO_RACOON_BBOX_WIDTH - MARIO_BIG_BBOX_WIDTH;
			}
			mario->SetState(MARIO_STATE_WALKING_LEFT);
		}
	}
	else if (game->IsKeyDown(DIK_DOWN))
	{
		mario->KeyDownPressed = 1;
		if (mario->getLevel() != MARIO_LEVEL_SMALL && !mario->holding)
			mario->SetState(MARIO_STATE_DUCK);
	}
	else if (game->IsKeyDown(DIK_UP))
	{
		mario->KeyUpPressed = 1;
	}
	else
	{
		mario->KeyUpPressed = 0;
		mario->KeyDownPressed = 0;
		mario->SetState(MARIO_STATE_IDLE);	
	}

}

vector<int> CPlayScene::getNum(int number)
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

void CPlayScene::UpdateBoardInfo(float camx)
{
	CGame* game = CGame::GetInstance();
	float xCoin = camx + 180.0f;
	vector<int> temp;
	temp = getNum(game->GetCoin());
	for (int i = 0, j = 0; i < int(numCoin.size()); i++, j++)
	{
		if (j < int(temp.size()))
			numCoin[i]->Update(xCoin, board->y + 17, temp[j]);
		else
			numCoin[i]->Update(xCoin, board->y + 17, 0);
		xCoin -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetTime());
	float xTime = camx + 180.0f;
	for (int i = numTime.size() - 1, j = 0; i >= 0; i--, j++)
	{
		if (j < int(temp.size()))
			numTime[i]->Update(xTime, board->y + 25, temp[j]);
		else
			numTime[i]->Update(xTime, board->y + 25, 0);
		xTime -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetLive());
	float xLive = camx + 77.0f;
	for (int i = numLive.size() - 1, j = 0; i >= 0; i--, j++)
	{
		if (j < int(temp.size()))
			numLive[i]->Update(xLive, board->y + 25, temp[j]);
		else
			numLive[i]->Update(xLive, board->y + 25, 0);
		xLive -= NUMBER_WIDTH;
	}
	temp.clear();
	temp = getNum(game->GetScore());
	float xScore = camx + 140.0f;
	for (int i = 0, j = 0; i < int(numScore.size()); i++, j++)
	{
		if (j < int(temp.size()))
			numScore[i]->Update(xScore, board->y + 25, temp[j]);
		else
			numScore[i]->Update(xScore, board->y + 25, 0);
		xScore -= NUMBER_WIDTH;
	}

	float xSpeedBar = camx + 92.0f;
	speedBar->Update(xSpeedBar, board->y + 17, player->speedStack);

	temp.clear();
	temp = game->GetItemList();
	float xItemList = camx + 207.0f;
	for (int i = 0; i < int(itemList.size()); i++)
	{
		itemList[i]->SetState(temp[i]);
		itemList[i]->SetPosition(xItemList, board->y + 16);
		xItemList += 24.0f;
	}
}

void CPlayScene::TimeLapse()
{
	CGame* game = CGame::GetInstance();
	if (GetTickCount64() - Dtime > ONE_SEC)
	{
		Itime = 0;
		Dtime = 0;
		game->SubTime();
		Timing();
	}

	if (GetTickCount64() - TimeWaitToScene > ONE_SEC * 3 )
	{
		TimeWaitToScene = 0;
		isWaiting = 0;
	}

	if (GetTickCount64() - TimeBrokenBrickBackToBrick > TIME_BACK_TO_BRICK)
	{
		TimeBrokenBrickBackToBrick = 0;
		isBrokenBrickBackToBrick = 1;
	}
}

void CPlayScene::IsCollisionAABBWithEnemies()
{
	CGame* game = CGame::GetInstance();
	for (int i = 0; i < int(listEnemies.size()); i++)
	{
		CGameObject* enemy = dynamic_cast<CGameObject*> (listEnemies[i]);
		if (!enemy->isFinish && enemy->type != OBJECT_TYPE_KOOPAS && enemy->type != OBJECT_TYPE_FLYKOOPAS)
		{
			if (player->CheckAABB(enemy))
			{
				if (player->untouchable == 0 )
				{
					if (player->getLevel() > MARIO_LEVEL_BIG)
					{
						player->StartTransform_Racoon();
						player->level = MARIO_LEVEL_BIG;
						player->ResetState();
						player->StartUntouchable();
					}
					else if (player->getLevel() == MARIO_LEVEL_BIG)
					{
						player->level = MARIO_LEVEL_SMALL;
						player->ResetState();
						CMario::ToSmall(player->y);
						player->StartUntouchable();
					}
					else
						player->SetState(MARIO_STATE_DIE);
				}
			}
		}
	}

	for (int i = 0; i < int(listObjects.size()); i++)
	{
		CGameObject* enemy = dynamic_cast<CGameObject*> (listObjects[i]);
		if (!enemy->isFinish)
		{
			if (enemy->type == OBJECT_TYPE_GOOMBA || enemy->type == OBJECT_TYPE_FLYGOOMBA)
			{
				if (player->CheckAABB(enemy))
				{
					if (player->untouchable == 0)
					{
						if (player->getLevel() > MARIO_LEVEL_BIG)
						{
							player->StartTransform_Racoon();
							player->level = MARIO_LEVEL_BIG;
							player->ResetState();
							player->StartUntouchable();
						}
						else if (player->getLevel() == MARIO_LEVEL_BIG)
						{
							player->level = MARIO_LEVEL_SMALL;
							player->ResetState();
							CMario::ToSmall(player->y);
							player->StartUntouchable();
						}
						else
							player->SetState(MARIO_STATE_DIE);
					}
				}
			}
		}
	}

	for (int i = 0; i < int(createObjects.size()); i++)
	{
		CGameObject* object = dynamic_cast<CGameObject*> (createObjects[i]);
		if (!object->isFinish)
		{
			if (object->type == OBJECT_TYPE_PLANTFIREBALL || object->type == OBJECT_TYPE_BOOMERANG)
			{
				if (player->untouchable == 0)
				{
					if (player->CheckAABB(object))
					{
						if (player->getLevel() > MARIO_LEVEL_BIG)
						{
							player->StartTransform_Racoon();
							player->level = MARIO_LEVEL_BIG;
							player->ResetState();
							player->StartUntouchable();
						}
						else if (player->getLevel() == MARIO_LEVEL_BIG)
						{
							player->level = MARIO_LEVEL_SMALL;
							player->ResetState();
							CMario::ToSmall(player->y);
							player->StartUntouchable();
						}
						else
							player->SetState(MARIO_STATE_DIE);
					}
				}
			}
		}
	}
}

void CPlayScene::IsCollisionAABBWithItems()
{
	CGame* game = CGame::GetInstance();
	for (int i = 0; i < int(listObjects.size()); i++)
	{
		CGameObject* object = dynamic_cast<CGameObject*> (listObjects[i]);
		if ((!object->isFinish) && (object->type == OBJECT_TYPE_COIN))
		{
			if (player->CheckAABB(object))
			{
				object->isFinish = 1;
				game->AddScore(100);
				game->AddCoin();
			}
		}
	}
	for (int i = 0; i < int(createObjects.size()); i++)
	{
		CGameObject* object = dynamic_cast<CGameObject*> (createObjects[i]);
		if (!object->isFinish)
		{
			if (object->type == OBJECT_TYPE_LEAF)
			{
				if (player->CheckAABB(object))
				{
					object->isFinish = 1;
					if (player->level == MARIO_LEVEL_SMALL)
					{
						CMario::ToBig(player->y);
						player->level = MARIO_LEVEL_BIG;
						player->StartTransform();
					}
					else if (player->getLevel() != MARIO_LEVEL_RACOON)
					{
						player->StartTransform_Racoon();
						player->level = MARIO_LEVEL_RACOON;
					}
					game->AddScore(1000);
				}
			}
			else if (object->type == OBJECT_TYPE_MUSHROOM)
			{
				if (player->CheckAABB(object))
				{
					object->isFinish = 1;
					if (player->getLevel() < MARIO_LEVEL_BIG)
					{
						CMario::ToBig(player->y);
						player->level = MARIO_LEVEL_BIG;
						player->StartTransform();
					}
					game->AddScore(1000);
				}
			}
			else if (object->type == OBJECT_TYPE_MUSHROOM_GREEN)
			{
				if (player->CheckAABB(object))
				{
					object->isFinish = 1;
					game->AddLive(1);
				}
			}
			else if (object->type == OBJECT_TYPE_FLOWER)
			{
				if (player->CheckAABB(object))
				{
					object->isFinish = 1;
					if (player->getLevel() == MARIO_LEVEL_SMALL)
					{
						CMario::ToBig(player->y);
						player->level = MARIO_LEVEL_BIG;
						player->StartTransform();
					}
					else if (player->getLevel() != MARIO_LEVEL_FIRE)
					{
						player->StartTransform_Racoon();
						player->level = MARIO_LEVEL_FIRE;
					}
					game->AddScore(1000);
				}
			}
		}
	}
}

void CPlayScene::IsOutOfMoveBar()
{
	if (player != NULL)
		if (player->isInMoveBar)
		{
			for (int i = 0; i < int(bar.size()); i++)
			{
				if (!bar[i]->isFinish)
					if (bar[i]->isStomped)
					{
						if (player->getLevel() == MARIO_LEVEL_RACOON)
						{
							if ((player->x + MARIO_RACOON_BBOX_WIDTH - MARIO_BIG_BBOX_WIDTH > bar[i]->x + BAR_BBOX_WIDTH) || (player->x + MARIO_RACOON_BBOX_WIDTH < bar[i]->x))
							{
								player->isInMoveBar = 0;
								bar[i]->isStomped = 0;
							}
						}
						else if (player->getLevel() == MARIO_LEVEL_BIG || player->getLevel() == MARIO_LEVEL_FIRE)
						{
							if ((player->x > bar[i]->x + BAR_BBOX_WIDTH) || (player->x + MARIO_BIG_BBOX_WIDTH < bar[i]->x))
							{
								player->isInMoveBar = 0;
								bar[i]->isStomped = 0;
							}
						}
						else
						{
							if ((player->x > bar[i]->x + BAR_BBOX_WIDTH) || (player->x + MARIO_SMALL_BBOX_WIDTH < bar[i]->x))
							{
								player->isInMoveBar = 0;
								bar[i]->isStomped = 0;
							}
						}
					}
			}
		}
}

void CPlayScene::ClearBarStomped()
{
	for (int i = 0; i < int(bar.size()); i++)
	{
		if (!bar[i]->isFinish)
			if (bar[i]->isStomped)
				bar[i]->isStomped = 0;
	}
}

void CPlayScene::UpdateCamera(float cx, float cy, int id)
{
	CGame* game = CGame::GetInstance();
	float camx, camy;
	game->GetCamPos(camx, camy);
	if (id == MAP_11)
	{
		if (cx < game->GetScreenWidth() / 2)
		{
			if (cy < 50.0f)
			{
				CGame::GetInstance()->SetCamPos(0.0f, 0.0f);
			}
			else if (cy < 280.0f)
			{
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(0.0f, round(cy));
			}
			else if (cy > 230 && cy < 470)
				CGame::GetInstance()->SetCamPos(0.0f, 230.0f); // set Cam when game start
			else if (cy > 470 && cy < 672)					// Cam in other screen
			{
				cy -= game->GetScreenHeight() / 2;
				CGame::GetInstance()->SetCamPos(0.0f, 470.0f);
			}
		}
		else if (cx > 2661.0f)
		{
			if (cy < 50.0f)
			{
				CGame::GetInstance()->SetCamPos(2508.0f, 0.0f);
			}
			else if (cy < 280.0f)
			{
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(2508.0f, round(cy));
			}
			else
				CGame::GetInstance()->SetCamPos(2508.0f, 230.0f); //set Cam when game end
		}
		else
		{
			if (cy < 50.0f)
			{
				cx -= game->GetScreenWidth() / 2;
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(round(cx), 0.0f);
			}
			else if (cy < 280.0f)
			{
				cx -= game->GetScreenWidth() / 2;
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(round(cx), round(cy));
			}
			else if (cy > 230 && cy < 470)
			{
				cx -= game->GetScreenWidth() / 2;
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(round(cx), 230.0f); // set Cam Focus
			}
			else if (cy > 470 && cy < 672)					// Cam in other screen
			{
				cx -= game->GetScreenWidth() / 2;
				cy -= 50.0f;
				CGame::GetInstance()->SetCamPos(round(cx), 470.0f);
			}
		}
	}
	else if (id == MAP_14)
	{
		if (cx > 2034.0f)
		{
			if (cx < 2304.0f + game->GetScreenWidth() / 2)
			{
				if (cy < 50.0f)
				{
					CGame::GetInstance()->SetCamPos(2304.0f, 0.0f);
				}
				else if (cy < 280.0f)
				{
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(2304.0f, round(cy));
				}
				else if (cy > 230 && cy < 470)
					CGame::GetInstance()->SetCamPos(2304.0f, 230.0f); // set Cam when game start
			}
			else if (cx > 2661.0f)
			{ 
				if (cy < 50.0f)
				{
					CGame::GetInstance()->SetCamPos(2508.0f, 0.0f);
				}
				else if (cy < 280.0f)
				{
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(2508.0f, round(cy));
				}
				else
					CGame::GetInstance()->SetCamPos(2508.0f, 230.0f); //set Cam when game end
			}
			else
			{
				if (cy < 50.0f)
				{
					cx -= game->GetScreenWidth() / 2;
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(round(cx), 0.0f);
				}
				else if (cy < 280.0f)
				{
					cx -= game->GetScreenWidth() / 2;
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(round(cx), round(cy));
				}
				else if (cy > 230 && cy < 470)
				{
					cx -= game->GetScreenWidth() / 2;
					cy -= 50.0f;
					CGame::GetInstance()->SetCamPos(round(cx), 230.0f); // set Cam Focus
				}
			}
		}
		else
		{
			if (temp == 0)
				temp = 1.0f;
			else
				temp = 0;

			if (camx < 1728.0f)
				CGame::GetInstance()->SetCamPos(round(camx + temp), 230.0f);
		}
	}
}