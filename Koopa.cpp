#include "Koopa.h"
#include "Mario.h"
#include "CBlock.h"
#include "Goomba.h"

void Koopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + KOOPA_BBOX_WIDTH;

	if (state == KOOPA_STATE_DIE)
		bottom = y + KOOPA_BBOX_HEIGHT_DIE;
	else if (state == KOOPA_STATE_DIE_HOLD)
		bottom = y + KOOPA_BBOX_HEIGHT_HOLD;
	else if (state == KOOPA_STATE_DIE_RUN_RIGHT || state == KOOPA_STATE_DIE_RUN_LEFT)
		bottom = y + KOOPA_BBOX_HEIGHT_RUN;
	else
		bottom = y + KOOPA_BBOX_HEIGHT;
}

void Koopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 
	
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if (state != KOOPA_STATE_DIE)
	{
		CalcPotentialCollisions(coObjects, coEvents);

		if (state == KOOPA_STATE_DIE_RUN_RIGHT || state == KOOPA_STATE_DIE_RUN_LEFT)
		{


			// No collision occured, proceed normally
			if (coEvents.size() == 0)
			{
				x += dx;
				y += dy;
				vy += KOOPA_GRAVITY * dt;
			}
			else
			{
				float min_tx, min_ty, nx = 0, ny;

				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);

				float y0, x0;
				x0 = x;
				y0 = y;
				float vx0 = vx;

				x = x0 + min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
				y = y0 + min_ty * dy + ny * 0.4f;

				if (ny != 0) vy = 0;
				if (nx != 0)
				{
					for (UINT i = 0; i < coEventsResult.size(); i++)
					{
						LPCOLLISIONEVENT e = coEventsResult[i];
						if (dynamic_cast<CMario*>(e->obj))
						{
							CMario* mario = dynamic_cast<CMario*>(e->obj);
							x = x + -min_tx * dx - nx * 0.4f + dx;
							y = y + -min_ty * dy - ny * 0.4f + dy;
							vx = vx0;
						}
						else if (dynamic_cast<CBlock*>(e->obj))
						{
							x = x + dx - min_tx * dx - nx * 0.4f;
						}
						else if (dynamic_cast<CGoomba*>(e->obj))
						{
							x = x + dx - min_tx * dx - nx * 0.4f;
						}
						else vx = -vx;
					}
				}

			}
		}
		else if (state == KOOPA_STATE_DIE_HOLD)
		{
			vector<LPCOLLISIONEVENT> coEvents;
			vector<LPCOLLISIONEVENT> coEventsResult;

			coEvents.clear();
			CalcPotentialCollisions(coObjects, coEvents);

			if (coEvents.size() == 0)
			{
				y += dy;
				x += dx;
			}
			else
			{
				float min_tx, min_ty, nx = 0, ny = 0;

				FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny);
				float x0 = x;
				float y0 = y;
				float vx0 = vx;

				for (UINT i = 0; i < coEventsResult.size(); i++)
				{
					LPCOLLISIONEVENT e = coEventsResult[i];
					if (dynamic_cast<CMario*>(e->obj)) 
					{
						CMario* mario = dynamic_cast<CMario*>(e->obj);
						if (mario->GetVX() > 0)
						{
							//vx = -KOOPA_HOLD;
							if (mario->GetLevel() == MARIO_LEVEL_RACCOON)
							{
								x = mario->GetX() + MARIO_RACCOON_BBOX_WIDTH  + 1.0f;
							}
							else x = mario->GetX() + MARIO_BIG_BBOX_WIDTH + 0.4f;
							y = mario->GetY();
						}
						if (mario->GetVX() < 0)
						{
							//vx = KOOPA_HOLD;
							x = mario->GetX() - KOOPA_BBOX_WIDTH - 0.4f;
							y = mario->GetY();
						}
						
					}

				}
			}
		}

	}


}

void Koopa::Render()
{
	if (state==KOOPA_STATE_WALKING_RIGHT) 
	{
		ani = KOOPA_ANI_WALKING_RIGHT;
	}
	if (state==KOOPA_STATE_WALKING_LEFT)
	{
		ani = KOOPA_ANI_WALKING_LEFT;
	}
	if (state == KOOPA_STATE_DIE) {
		ani = KOOPA_ANI_DIE;
	}
	if (state == KOOPA_STATE_DIE_HOLD) {
		ani = KOOPA_ANI_DIE_HOLD;
	}
	if (state == KOOPA_STATE_DIE_RUN_RIGHT || state == KOOPA_STATE_DIE_RUN_LEFT) {
		ani = KOOPA_ANI_DIE_RUN;
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
	case KOOPA_STATE_WALKING_LEFT:
		vx = -KOOPA_WALKING_SPEED;
		nx = -1;
		break;
	case KOOPA_STATE_WALKING_RIGHT:
		vx = KOOPA_WALKING_SPEED;
		nx = 1;
		break;
	case KOOPA_STATE_DIE_RUN_RIGHT:
		nx = 1;
		y = y - 5;
		vx = KOOPA_DIE_RUN_SPEED;
		vy = KOOPA_WALKING_SPEED;
		break;

	case KOOPA_STATE_DIE_RUN_LEFT:
		nx = -1;
		y = y - 5;
		vx = -KOOPA_DIE_RUN_SPEED;
		vy = KOOPA_WALKING_SPEED;
		break;
	}
}