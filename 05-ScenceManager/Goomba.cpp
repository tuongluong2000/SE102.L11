#include "Goomba.h"
CGoomba::CGoomba()
{
	SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float &left, float &top, float &right, float &bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else 	
		bottom = y + GOOMBA_BBOX_HEIGHT;
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 


	if (vx < 0 && x < 0) {
		x = 0; vx = -vx;
	}

	if (vx > 0 && x > 290) {
		x = 290; vx = -vx;
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if (GetTickCount() - die_start > GOOMBA_TIME_DIE && die_start !=NULL)
	{
		SetState(GOOMBA_STATE_DIE_TIME);
		die_start = NULL;
		return;
	}
	// turn off collision when die 
	if (state != GOOMBA_STATE_DIE && state!= GOOMBA_STATE_DIE_TIME)
		CalcPotentialCollisions(coObjects, coEvents);
	else return;

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{

		x += dx;
		y += dy;
		

	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		float y0, x0;
		x0 = x;
		y0 = y;
		float vx0 = vx;

		// block 

		x = x0 + min_tx * dx + nx * 0.4f;
		y = y0 + min_ty * dy + ny * 0.4f;

		//if (nx != 0) vx = -vx;
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
					mario->LevelDie();
					vx = vx0;
				}
				else if (dynamic_cast<CKoopas*>(e->obj))
				{
					CKoopas* koopa = dynamic_cast<CKoopas*>(e->obj);
					if (koopa->GetState() == KOOPA_STATE_DIE_RUN_RIGHT || koopa->GetState() == KOOPA_STATE_DIE_RUN_LEFT)
						SetState(GOOMBA_STATE_DIE);
				}
				else vx = -vx;
			}
		}
	}
}

void CGoomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) {
		ani = GOOMBA_ANI_DIE;
	}
	if (state != GOOMBA_STATE_DIE_TIME)
		animation_set->at(ani)->Render(x,y);

	//RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
		case GOOMBA_STATE_DIE:
			y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
			die_start = GetTickCount();
			vx = 0;
			vy = 0;
			break;
		case GOOMBA_STATE_WALKING: 
			vx = -GOOMBA_WALKING_SPEED;
			break;
		case GOOMBA_STATE_DIE_TIME:
			die = true;
			break;

	}
}
