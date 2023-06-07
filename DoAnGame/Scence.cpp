#include "Scence.h"

CScene::CScene(int id, LPCWSTR filePath)
{
	this->key_handler = NULL;
	this->id = id;
	this->sceneFilePath = filePath;
}