#pragma once
#include "GameObject.h"

#define GOOMBA_WALKING_SPEED 0.03f;
#define GOOMBA_GRAVITY			0.002f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 15
#define GOOMBA_BBOX_HEIGHT_DIE 12

#define GOOMBA_STATE_WALKING 100
#define GOOMBA_STATE_DIE 200
#define GOOMBA_STATE_DIE_TIME 300


#define GOOMBA_ANI_WALKING 0
#define GOOMBA_ANI_DIE 1

#define GOOMBA_TIME_DIE 100

class CGoomba : public CGameObject
{
	virtual void GetBoundingBox(float &left, float &top, float &right, float &bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects);
	virtual void Render();


public: 	
	virtual void SetState(int state);
private:
	int a;
	DWORD die_start;
public:
	CGoomba(int a) { this->a = a; }
};