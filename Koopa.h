#pragma once
#include "GameObject.h"

#define KOOPA_WALKING_SPEED 0.03f;

#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 15
#define KOOPA_BBOX_HEIGHT_DIE 9

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_DIE 200

#define KOOPA_ANI_WALKING 0
#define KOOPA_ANI_WALKING1 2
#define KOOPA_ANI_DIE 1

class Koopa : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();


public:
	virtual void SetState(int state);
private:
	int a;
	int ani = KOOPA_ANI_WALKING;
public:
	Koopa(int a) { this->a = a; }
};