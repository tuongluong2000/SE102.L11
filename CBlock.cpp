#include "CBlock.h"

CBlock::CBlock(int idSprite, int typeBlock) {
	this->idSprite = idSprite;
	this->typeBlock = typeBlock;
}

CBlock::~CBlock() {}

void CBlock::Render()
{
	CSprites* sprites = CSprites::GetInstance();
	sprites->Get(idSprite)->Draw(x, y);
}

void CBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BLOCK_BBOX_WIDTH;
	b = y + BLOCK_BBOX_HEIGHT;
}
