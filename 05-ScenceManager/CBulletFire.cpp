#include "CBulletFire.h"
#include "Koopas.h"
#include "Mario.h"
#include "Goomba.h"

void CBulletFire::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();
	if (state != BULLETFIRE_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);
	else return;

	if (GetTickCount() - time_die > BULLETFIRE_TIME_DIE)
	{
		SetState(BULLETFIRE_STATE_DIE);
	}

	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
		return;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		if (nx != 0 || ny != 0)
		{
			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				LPCOLLISIONEVENT e = coEventsResult[i];
				if (dynamic_cast<CGoomba*>(e->obj))
				{
					CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						SetState(BULLETFIRE_STATE_DIE);
						return;
					}

				}
				if (dynamic_cast<CKoopas*>(e->obj))
				{
					CKoopas* koopa = dynamic_cast<CKoopas*>(e->obj);
					if (koopa->GetState() != KOOPA_STATE_DIE)
					{
						koopa->SetState(KOOPA_STATE_DIE);
						SetState(BULLETFIRE_STATE_DIE);
						return;
					}

				}
			}
		}
	}
	x += dx;
	y += dy;

}

void CBulletFire::Render()
{
	if (GetState() != BULLETFIRE_STATE_DIE)
	{
		animation_set->at(0)->Render(x, y);
		RenderBoundingBox();
	}
}


void CBulletFire::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BULLETFIRE_BBOX_WIDTH;
	b = y + BULLETFIRE_BBOX_HEIGHT;
}

void CBulletFire::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case BULLETFIRE_STATE_WALKING_RIGHT:
		vy = BULLETFIRE_VY;
		vx = BULLETFIRE_VX;
		time_die = GetTickCount();
		break;
	case BULLETFIRE_STATE_WALKING_LEFT:
		vy = BULLETFIRE_VY;
		vx = -BULLETFIRE_VX;
		time_die = GetTickCount();
		break;
	case BULLETFIRE_STATE_DIE:
		die = true;
		break;
	}
}