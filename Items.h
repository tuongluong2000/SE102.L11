#pragma once
#include "GameObject.h"

#define ITEMS_STAR_SPEED 0.15f;
#define ITEMS_MUSHROOM_UP_SPEED 0.01f;
#define ITEMS_MUSHROOM_RUN_SPEED 0.03f;
#define ITEMS_DIE_SPEED 0.05f;
#define ITEMS_LEAF_X_SPEED 0.05f;
#define ITEMS_LEAF_Y_SPEED 0.01f;

#define ITEMS_BBOX_WIDTH  16
#define ITEMS_BBOX_HEIGHT 16

#define ITEMS_STATE_WALKING 100
#define ITEMS_STATE_MUSHROOM 200
#define ITEMS_STATE_STAR 300
#define ITEMS_STATE_DIE 400
#define ITEMS_STATE_LEAF 500

#define ITEMS_ANI_WALKING 0
#define ITEMS_ANI_STAR 1
#define ITEMS_ANI_MUSHROOM 2
#define ITEMS_ANI_DIE 3
#define ITEMS_ANI_LEAF 4

class Items : public CGameObject
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();


public:
	virtual void SetState(int state);
	int GetB() { return b; }
	int GetA() { return a; }
private:
	int a;//0=items, 1 tem
	int b;//0= satr, 1=mushroom
	int x0;
	int y0;
	int ani = ITEMS_ANI_WALKING;
public:
	Items(int a, int b)
	{ 
		this->a = a;
		this->b = b;
	}
};

