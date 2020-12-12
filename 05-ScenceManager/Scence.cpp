#include "Scence.h"

CScene::CScene(int id, LPCWSTR filePath, LPCWSTR mapPath)
{
	this->id = id;
	this->sceneFilePath = filePath;
	this->mapPath = mapPath;
}