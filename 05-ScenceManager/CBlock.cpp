#include "CBlock.h"

CBlock::CBlock() {
	
}

CBlock::~CBlock() {}

void CBlock::Render()
{
	int ani = 0;
	animation_set->at(ani)->Render(x, y);
}

void CBlock::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BLOCK_BBOX_WIDTH;
	b = y + BLOCK_BBOX_HEIGHT;
}

