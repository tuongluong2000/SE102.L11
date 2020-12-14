#pragma once
#include "GameObject.h"

#define KOOPA_WALKING_SPEED 0.02;
#define KOOPA_DIE_WALKING_SPEED 0.03f;
#define KOOPA_DIE_RUN_SPEED 0.1f;
#define KOOPA_GRAVITY			0.002f
#define KOOPA_HOLD    0.02f


#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_BBOX_HEIGHT_DIE 16
#define KOOPA_BBOX_HEIGHT_HOLD 15
#define KOOPA_BBOX_HEIGHT_RUN 15


#define KOOPA_STATE_WALKING_RIGHT 100
#define KOOPA_STATE_WALKING_LEFT 200
#define KOOPA_STATE_DIE 300
#define KOOPA_STATE_DIE_HOLD 400
#define KOOPA_STATE_DIE_RUN_RIGHT 500
#define KOOPA_STATE_DIE_RUN_LEFT 600

#define KOOPA_ANI_WALKING_LEFT 1
#define KOOPA_ANI_WALKING_RIGHT 0
#define KOOPA_ANI_DIE 2
#define KOOPA_ANI_DIE_RUN 4
#define KOOPA_ANI_DIE_HOLD 3

class CKoopas : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();


public:
	virtual void SetState(int state);
	void SetVX(float Vx) { this->vx = Vx; }
private:
	int a;
public:
	CKoopas() {};

};