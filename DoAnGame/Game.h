#pragma once

#include <unordered_map>

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#define MAP_11	2
#define MAP_14	3

#define START_POSITION_X	65
#define START_POSITION_Y	63

#define MAP11_POSITION_X	97
#define MAP11_POSITION_Y	31

#define MAP14_POSITION_X	193
#define MAP14_POSITION_Y	63

#define LINE_OF_DEATH		434.0f


#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Scence.h"

using namespace std;

#define KEYBOARD_BUFFER_SIZE 1024

class CGame
{
	static CGame* __instance;
	HWND hWnd;									// Window handle

	LPDIRECT3D9 d3d = NULL;						// Direct3D handle
	LPDIRECT3DDEVICE9 d3ddv = NULL;				// Direct3D device object

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;			// Sprite helper library to help us draw 2D image on the screen 

	LPDIRECTINPUT8       di;		// The DirectInput object         
	LPDIRECTINPUTDEVICE8 didv;		// The keyboard device 

	BYTE  keyStates[256];			// DirectInput keyboard state buffer 
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];		// Buffered keyboard data

	LPKEYEVENTHANDLER keyHandler;

	float cam_x = 0.0f;
	float cam_y = 0.0f;

	int screen_width;
	int screen_height;

	unordered_map<int, LPSCENE> scenes;
	int current_scene;

	void _ParseSection_SETTINGS(string line);
	void _ParseSection_SCENES(string line);

	// Game play
	float miniMarioPositionX = START_POSITION_X;
	float miniMarioPositionY = START_POSITION_Y;

	int level = 1;
	int score = 0;
	int time = 0;
	int live = 2;
	int coin = 0;
	vector<int> ItemList{ 0, 0, 0 };

public:
	void InitKeyboard();
	void SetKeyHandler(LPKEYEVENTHANDLER handler) { keyHandler = handler; }
	void Init(HWND hWnd);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha = 255);

	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();

	void Load(LPCWSTR gameFile);
	LPSCENE GetCurrentScene() { return scenes[current_scene]; }
	void SwitchScene(int scene_id);
	int GetCurrentSceneID() { return current_scene; };

	int GetScreenWidth() { return screen_width; }
	int GetScreenHeight() { return screen_height; }

	static void SweptAABB(
		float ml,			// move left 
		float mt,			// move top
		float mr,			// move right 
		float mb,			// move bottom
		float dx,			// 
		float dy,			// 
		float sl,			// static left
		float st,
		float sr,
		float sb,
		float& t,
		float& nx,
		float& ny);

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddv; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	void SetMiniMarioPosition(float x, float y) { this->miniMarioPositionX = x; this->miniMarioPositionY = y; };
	float GetMiniMarioPositionX() { return this->miniMarioPositionX; };
	float GetMiniMarioPositionY() { return this->miniMarioPositionY; };

	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }
	void GetCamPos(float &x, float &y) { x = cam_x; y = cam_y; }

	void SetTime(int t) { time = t; }
	void SubTime() { time -= 1; }
	int GetTime() { return time; }

	int GetLevel() { return this->level; }
	void SetLevel(int level) { this->level = level; }

	void SetScore(int t) { score = t; }
	void AddScore(int t) { score += t; }
	int GetScore() { return score; }

	void SetLive(int t) { live = t; }
	void AddLive(int t) { live += t; }
	void SubLive() { live -= 1; }
	int GetLive() { return live; }

	void SetCoin(int t) { coin = t; }
	int GetCoin() { return coin; }
	void AddCoin() { 
		coin += 1; 
		if (coin == 100)	// 100 coin then add 1 live
		{
			AddLive(1);
			SetCoin(0);
		}
	}

	void ResetItemList()
	{
		for (int i = 0; i < int(ItemList.size()); i++)										// clear items
			ItemList[i] = 0;
	}
	vector<int> GetItemList() { return ItemList; }
	void PushItem(int item) {
		for (int i = 0; i < int(ItemList.size()); i++)
		{
			if (ItemList[i] == 0)
			{
				ItemList[i] = item;
				break;
			}
		}
		if (int(ItemList[0]) != 0 && int(ItemList[1]) != 0 && int(ItemList[2]) != 0)			// add live when enough itemlist
		{
			if (int(ItemList[0]) == 1 && int(ItemList[1]) == 1 && int(ItemList[2]) == 1)		// 3 star add 5 lives
				AddLive(5);
			else if (int(ItemList[0]) == 2 && int(ItemList[1]) == 2 && int(ItemList[2]) == 2)	// 3 mushroom add 2 lives
				AddLive(2);
			else if (int(ItemList[0]) == 3 && int(ItemList[1]) == 3 && int(ItemList[2]) == 3)	// 3 flower add 3 lives
				AddLive(3);
			else
				AddLive(1);																		// else add 1 live
			
			for (int i = 0; i < int(ItemList.size()); i++)										// clear items
				ItemList[i] = 0;
		}
	}


	static CGame* GetInstance();

	~CGame();
};


