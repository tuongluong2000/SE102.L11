#pragma once
#include "GameObject.h"

#define BLOCK_BBOX_WIDTH  16
#define BLOCK_BBOX_HEIGHT 16

class CBlock : public CGameObject
{
	int idSprite;
	int typeBlock;
public:
	CBlock();
	~CBlock();
	int GetTypeBlock() { return this->typeBlock; }
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
};
