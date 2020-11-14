#include "Koopa.h"

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + KOOPA_BBOX_WIDTH;

	if (state == KOOPA_STATE_DIE)
		bottom = y + KOOPA_BBOX_HEIGHT_DIE;
	else
		bottom = y + KOOPA_BBOX_HEIGHT;
}

void Koopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	x += dx;
	y += dy;

	if (a == 0)
	{
		if (vx > 0 && x > 290) {
			x = 290; vx = -vx;
		}
	}
	if (a == 1)
	{
		if (vx != 0 && x < 512)
		{
			x = 512; vx = -vx;
		}
		if (vx != 0 && x > 590)
		{
			x = 590; vx = -vx;
		}
	}
	if (a == 2)
	{
		if (vx != 0 && x < 672)
		{
			x = 672; vx = -vx;
		}
		if (vx != 0 && x > 1072)
		{
			x = 1072; vx = -vx;
		}
	}

}

void Koopa::Render()
{
	if (x <= 513)
	{
		ani = KOOPA_ANI_WALKING1;
	}
	if (x >= 589)
	{
		ani = KOOPA_ANI_WALKING;
	}
	if (state == KOOPA_STATE_DIE) {
		ani = KOOPA_ANI_DIE;
	}

	animations[ani]->Render(x, y);
	RenderBoundingBox();
}

void Koopa::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPA_STATE_DIE:
		y += KOOPA_BBOX_HEIGHT - KOOPA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		break;
	case KOOPA_STATE_WALKING:
		vx = -KOOPA_WALKING_SPEED;
	}
}