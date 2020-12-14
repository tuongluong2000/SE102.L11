#include "Items.h"


void Items::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + ITEMS_BBOX_WIDTH;
	b = y + ITEMS_BBOX_HEIGHT;
}

void Items::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEvents.clear();
	CalcPotentialCollisions(coObjects, coEvents);
	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 
	x += dx;
	y += dy;

	if (state == ITEMS_STATE_STAR)
	{

		if (y < y0 - 48)
		{
			vy = ITEMS_STAR_SPEED;
		}
		if (vy > 0 && y >= y0 - 16)
		{
			vy = 0;
			y = y0 - 16;
		}
	}
	if (state == ITEMS_STATE_DIE)
	{
		if (y < y0 - 8)
			vy = ITEMS_DIE_SPEED;
		if (vy > 0 && y >= y0)
		{
			vy = 0;
			y = y0;
		}
	}
	if (state == ITEMS_STATE_MUSHROOM)
	{


		if (y < y0 - 16)
		{
			vx = -ITEMS_MUSHROOM_UP_SPEED;
			vy = ITEMS_MUSHROOM_UP_SPEED;
		}
		if (y > y0 + 16)
		{

			if (coEvents.size() != 0)
			{
				float min_tx, min_ty, nx = 0, ny;
				float rdx = 0;
				float rdy = 0;

				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

				float y0, x0;
				x0 = x;
				y0 = y;

				// block 

				if (nx < 0) vx = 0;
				if (ny < 0) vy = 0;
			}
			else
			{
				vy = ITEMS_MUSHROOM_UP_SPEED;
			}
		}

	}
	if (state == ITEMS_STATE_LEAF)
	{
		if (y < y0 - 36)
		{
			vx = ITEMS_LEAF_X_SPEED;
			vy = ITEMS_LEAF_Y_SPEED;
		}
		if (x > x0 + 32)
		{
			vx = -ITEMS_LEAF_X_SPEED
		}
		if (x < x0 - 32)
		{
			vx = ITEMS_LEAF_X_SPEED
		}
	}

}

void Items::Render()
{

	if (state == ITEMS_STATE_STAR) {
		ani = ITEMS_ANI_STAR;
	}
	else
	{
		if (state == ITEMS_STATE_MUSHROOM) {
			ani = ITEMS_ANI_MUSHROOM;
		}
		else
		{
			if (state == ITEMS_STATE_DIE)
				ani = ITEMS_ANI_DIE;
			else
			{
				if (state == ITEMS_STATE_LEAF)
					ani = ITEMS_ANI_LEAF;
			}
		}
	}

	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void Items::SetState(int state)
{

	CGameObject::SetState(state);

	switch (state)
	{
	case ITEMS_STATE_STAR:
		vx = 0;
		vy = -ITEMS_STAR_SPEED;
		y0 = y;
		x0 = x;
		break;
	case ITEMS_STATE_MUSHROOM:
		vx = 0;
		vy = -ITEMS_MUSHROOM_UP_SPEED;
		y0 = y;
		x0 = x;
		break;
	case ITEMS_STATE_DIE:
		vx = 0;
		vy = -ITEMS_DIE_SPEED;
		y0 = y;
		x0 = x;
		break;
	case ITEMS_STATE_LEAF:
		vx = 0;
		vy = -ITEMS_LEAF_Y_SPEED;
		y0 = y;
		x0 = x;
	}




}
