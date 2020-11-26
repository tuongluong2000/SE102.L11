#pragma once
#include "GameObject.h"

#define BULLETFIRE_BBOX_WIDTH  10
#define BULLETFIRE_BBOX_HEIGHT 10

#define BULLETFIRE_STATE_WALKING_RIGHT 100
#define BULLETFIRE_STATE_WALKING_LEFT 2000
#define BULLETFIRE_STATE_DIE	300

#define BULLETFIRE_ANI_WALKING_RIGHT 0
#define BULLETFIRE_ANI_WALKING_LEFT 1

#define BULLETFIRE_VY 0.01f
#define BULLETFIRE_VX 0.15f

#define BULLETFIRE_TIME_DIE 3000


class CBulletFire : public CGameObject
{
public:
	CBulletFire() : CGameObject()
	{}
	virtual void Render();
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) ;
	virtual void GetBoundingBox(float &l, float &t, float &r, float &b);
	virtual void SetState(int state);
	void SetVX(float vx) { this->vx = vx; }
	~CBulletFire();
private:
	DWORD time_die;
};

