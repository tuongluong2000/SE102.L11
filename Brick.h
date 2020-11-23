#pragma once
#include "GameObject.h"

#define BRICK_BBOX_WIDTH  16
#define BRICK_BBOX_HEIGHT 16

class CBrick;
typedef CBrick* LPCBRICK;

class CBrick : public CGameObject
{
public:
	virtual void Render();
	//virtual void Update(DWORD dt, vector<LPCBRICK>* coObjects = NULL) ;
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
};